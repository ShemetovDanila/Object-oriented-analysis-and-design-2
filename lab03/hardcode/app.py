import tkinter as tk
from tkinter import ttk, messagebox
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import psutil
import time
import threading
import os
import sys
from collections import deque
from typing import Dict, List, Optional

# настройки
HISTORY_SIZE = 60
TICK_INTERVAL_MS = 1000
LOG_PATH = "metrics.prom"
MAX_ALERTS = 50

# глобальное состояние
_global_state = {
    "overrides": {},
    "lock": threading.RLock(),
    "log_file": None,
    "log_host": "localhost",
    "active_alerts": [],
    "dashboard_history": None,
    "dashboard_plots": None
}

# сбор метрик
def collect_metrics() -> Dict[str, float]:
    try:
        cpu = psutil.cpu_percent(interval=0.1)
    except Exception:
        cpu = 0.0
    try:
        ram = psutil.virtual_memory().percent
    except Exception:
        ram = 0.0
    try:
        disk = psutil.disk_usage(os.getcwd()).percent
    except Exception:
        disk = 0.0
    return {"cpu": cpu, "ram": ram, "disk": disk}

# применение переопределений
def apply_overrides(raw: Dict[str, float]) -> Dict[str, float]:
    with _global_state["lock"]:
        return {k: _global_state["overrides"].get(k, v) for k, v in raw.items()}

def set_override(key: str, value: float) -> None:
    with _global_state["lock"]:
        _global_state["overrides"][key] = value

def clear_override(key: str) -> None:
    with _global_state["lock"]:
        _global_state["overrides"].pop(key, None)

def get_overrides() -> Dict[str, float]:
    with _global_state["lock"]:
        return _global_state["overrides"].copy()

# логирование
def init_logger(path: str, host: str = "localhost") -> None:
    try:
        _global_state["log_file"] = open(path, "a", encoding="utf-8")
        _global_state["log_host"] = host
    except OSError as e:
        raise RuntimeError(f"Cannot open log file {path}: {e}")

def log_metrics(metrics: Dict[str, float], timestamp: float) -> None:
    file = _global_state.get("log_file")
    if not file or file.closed:
        return
    try:
        ts_ms = int(timestamp * 1000)
        host = _global_state.get("log_host", "localhost")
        for name, val in metrics.items():
            file.write(f'system_{name}{{host="{host}"}} {val:.2f} {ts_ms}\n')
        file.flush()
    except OSError as e:
        print(f"[Logger] Disk/IO error: {e}", file=sys.stderr)

def close_logger() -> None:
    file = _global_state.get("log_file")
    if file and not file.closed:
        file.close()

# система алертов
class AlertRule:
    def __init__(self, master: tk.Tk, metric: str, threshold: float, message: str, cooldown: int = 30):
        self.master = master
        self.metric = metric
        self.threshold = threshold
        self.message = message
        self.cooldown = cooldown
        self._last_triggered = 0.0
        self._active_popup: Optional[tk.Toplevel] = None

    def check(self, metrics: Dict[str, float], timestamp: float) -> None:
        val = metrics.get(self.metric)
        if val is None or val < self.threshold:
            return
        if (timestamp - self._last_triggered) < self.cooldown:
            return
        self._last_triggered = timestamp
        self._show_alert(val)

    def _show_alert(self, val: float) -> None:
        if self._active_popup is not None and self._active_popup.winfo_exists():
            return
        popup = tk.Toplevel(self.master)
        popup.title("⚠️ SYSTEM ALERT")
        popup.geometry("380x180")
        popup.configure(bg="#1e1e2e")
        popup.attributes("-topmost", True)
        self._active_popup = popup
        tk.Label(popup, text="🚨 ALERT TRIGGERED", fg="#f38ba8", bg="#1e1e2e", font=("Segoe UI", 12, "bold")).pack(pady=10)
        tk.Label(popup, text=self.message, fg="#cdd6f4", bg="#1e1e2e", wraplength=340, justify="center").pack(pady=5)
        tk.Label(popup, text=f"Current: {val:.1f}% | Threshold: ≥{self.threshold:.0f}%", fg="#a6adc8", bg="#1e1e2e").pack()
        tk.Button(popup, text="ACKNOWLEDGE", bg="#f38ba8", fg="#11111b", font=("Segoe UI", 10, "bold"), relief="flat", command=self.dismiss).pack(pady=10)
        popup.protocol("WM_DELETE_WINDOW", self.dismiss)

    def dismiss(self):
        if self._active_popup and self._active_popup.winfo_exists():
            self._active_popup.destroy()
        self._active_popup = None

def check_all_alerts(metrics: Dict[str, float], timestamp: float) -> None:
    for rule in _global_state["active_alerts"]:
        try:
            rule.check(metrics, timestamp)
        except Exception as e:
            print(f"[Alert Error] {e}", file=sys.stderr)

# обновление дашборда
def update_dashboard_plots(metrics: Dict[str, float]) -> None:
    plots = _global_state.get("dashboard_plots")
    history = _global_state.get("dashboard_history")
    if not plots or not history:
        return
    for key, val in metrics.items():
        if key not in history:
            continue
        history[key].append(val)
        x = list(range(len(history[key])))
        plots["lines"][key].set_data(x, history[key])
        plots["axes"][key].set_xlim(0, HISTORY_SIZE)
        plots["axes"][key].set_ylim(0, 100)
        plots["canvases"][key].draw_idle()

# gui дашборда
class DashboardGUI:
    def __init__(self, root: tk.Tk):
        self.root = root
        self._setup_ui()
        _global_state["dashboard_history"] = {m: deque(maxlen=HISTORY_SIZE) for m in ["cpu", "ram", "disk"]}
        _global_state["dashboard_plots"] = {"lines": self.lines, "axes": self.axes, "canvases": self.canvases}

    def _setup_ui(self):
        self.root.title("📊 Real-Time System Dashboard")
        self.root.geometry("880x920")
        self.root.configure(bg="#11111b")
        style = ttk.Style()
        style.theme_use("clam")
        style.configure("TFrame", background="#11111b")
        style.configure("TLabel", background="#11111b", foreground="#cdd6f4", font=("Segoe UI", 11))
        style.configure("TButton", background="#313244", foreground="#cdd6f4", font=("Segoe UI", 10))
        titles = {"cpu": "🖥️ CPU USAGE", "ram": "🧠 RAM USAGE", "disk": "💾 DISK USAGE"}
        self.lines, self.axes, self.canvases = {}, {}, {}
        for key, title in titles.items():
            frame = ttk.Frame(self.root)
            frame.pack(fill="x", padx=15, pady=8)
            ttk.Label(frame, text=title, font=("Segoe UI", 12, "bold")).pack(anchor="w", pady=(0, 4))
            fig = Figure(figsize=(7.8, 2.2), dpi=100)
            fig.patch.set_facecolor("#11111b")
            ax = fig.add_subplot(111)
            ax.set_facecolor("#1e1e2e")
            ax.set_ylim(0, 100)
            ax.set_xlim(0, HISTORY_SIZE)
            for spine in ax.spines.values():
                spine.set_color("#45475a")
            ax.tick_params(colors="#a6adc8")
            line, = ax.plot([], [], color="#89b4fa", linewidth=2)
            canvas = FigureCanvasTkAgg(fig, master=frame)
            canvas.draw()
            canvas.get_tk_widget().pack(fill="x", expand=True)
            tk.Button(frame, text="🔔 Configure Alert", bg="#313244", fg="#cdd6f4", font=("Segoe UI", 10), relief="flat", cursor="hand2", command=lambda k=key: self._open_alert_dialog(k)).pack(pady=4, anchor="e")
            self.lines[key] = line
            self.axes[key] = ax
            self.canvases[key] = canvas

    def _open_alert_dialog(self, metric: str) -> None:
        remaining = MAX_ALERTS - len(_global_state["active_alerts"])
        if remaining <= 0:
            messagebox.showwarning("Limit Reached", f"Maximum {MAX_ALERTS} alert rules reached.", parent=self.root)
            return
        dlg = tk.Toplevel(self.root)
        dlg.title(f"⚙️ Alert Rule: {metric.upper()} ({remaining} left)")
        dlg.geometry("320x250")
        dlg.configure(bg="#181825")
        dlg.transient(self.root)
        dlg.grab_set()
        tk.Label(dlg, text="Threshold (%)", fg="#cdd6f4", bg="#181825", font=("Segoe UI", 11)).pack(pady=(15, 0))
        thr_entry = ttk.Entry(dlg, width=10, justify="center")
        thr_entry.insert(0, "90")
        thr_entry.pack(pady=5)
        tk.Label(dlg, text="Alert Message", fg="#cdd6f4", bg="#181825", font=("Segoe UI", 11)).pack(pady=(10, 0))
        msg_entry = ttk.Entry(dlg, width=35, justify="center")
        msg_entry.insert(0, f"⚠️ {metric.upper()} usage exceeded safe limit!")
        msg_entry.pack(pady=5)
        def save():
            try:
                threshold = float(thr_entry.get())
                message = msg_entry.get().strip() or f"{metric} alert"
                rule = AlertRule(self.root, metric, threshold, message, cooldown=30)
                _global_state["active_alerts"].append(rule)
                messagebox.showinfo("✅ Success", f"Alert for {metric} activated.", parent=dlg)
                dlg.destroy()
            except ValueError:
                messagebox.showerror("❌ Error", "Threshold must be a number.", parent=dlg)
        tk.Button(dlg, text="💾 Save & Subscribe", bg="#89b4fa", fg="#11111b", font=("Segoe UI", 11, "bold"), relief="flat", command=save).pack(pady=15)

    def cleanup(self):
        for rule in _global_state["active_alerts"]:
            rule.dismiss()
        _global_state["active_alerts"].clear()

# gui админ-панели
class AdminPanel:
    def __init__(self, parent: tk.Tk):
        self.win = tk.Toplevel(parent)
        self.win.title("🛠️ Admin Override Console")
        self.win.geometry("440x400")
        self.win.configure(bg="#181825")
        self._setup_ui()

    def _setup_ui(self):
        style = ttk.Style()
        style.configure("Admin.TFrame", background="#181825")
        style.configure("Admin.TLabel", background="#181825", foreground="#cdd6f4", font=("Segoe UI", 11))
        style.configure("Admin.TCheckbutton", background="#181825", foreground="#cdd6f4")
        style.configure("Admin.TScale", background="#181825", troughcolor="#313244", slidercolor="#89b4fa")
        style.configure("Admin.TButton", background="#313244", foreground="#cdd6f4")
        tk.Label(self.win, text="OVERRIDE CONTROLS", font=("Segoe UI", 14, "bold"), fg="#cdd6f4", bg="#181825").pack(pady=10)
        self.controls = {}
        for metric in ["cpu", "ram", "disk"]:
            frame = ttk.LabelFrame(self.win, text=metric.upper(), padding=10)
            frame.pack(fill="x", padx=20, pady=5)
            var = tk.BooleanVar(value=False)
            ttk.Checkbutton(frame, text="⚡ Enable Override", variable=var, command=lambda m=metric: self._sync(m)).pack(anchor="w")
            scale = ttk.Scale(frame, from_=0, to=100, orient="horizontal", length=340, command=lambda v, m=metric: self._on_slide(m, v))
            scale.pack(pady=5)
            self.controls[metric] = {"var": var, "scale": scale}
        tk.Button(self.win, text="🔄 Sync UI from Engine", bg="#313244", fg="#cdd6f4", font=("Segoe UI", 10), relief="flat", command=self._sync_all_ui).pack(pady=8)
        self._sync_all_ui()

    def _on_slide(self, metric: str, val: str) -> None:
        if self.controls[metric]["var"].get():
            set_override(metric, float(val))

    def _sync(self, metric: str) -> None:
        if self.controls[metric]["var"].get():
            set_override(metric, self.controls[metric]["scale"].get())
        else:
            clear_override(metric)

    def _sync_all_ui(self):
        overrides = get_overrides()
        for metric, ctrl in self.controls.items():
            if metric in overrides:
                ctrl["var"].set(True)
                ctrl["scale"].set(overrides[metric])
            else:
                ctrl["var"].set(False)

# основной цикл
def tick_loop(root: tk.Tk) -> None:
    try:
        raw = collect_metrics()
        current = apply_overrides(raw)
        ts = time.time()
        log_metrics(current, ts)
        check_all_alerts(current, ts)
        update_dashboard_plots(current)
    except Exception as e:
        print(f"[Tick Error] {e}", file=sys.stderr)
    finally:
        root.after(TICK_INTERVAL_MS, lambda: tick_loop(root))

def shutdown(root: tk.Tk, dashboard: DashboardGUI) -> None:
    dashboard.cleanup()
    close_logger()
    root.destroy()

# точка входа
if __name__ == "__main__":
    try:
        root = tk.Tk()
        init_logger(LOG_PATH, host="monitor-01")
        dashboard = DashboardGUI(root)
        admin = AdminPanel(root)
        root.protocol("WM_DELETE_WINDOW", lambda: shutdown(root, dashboard))
        root.after(TICK_INTERVAL_MS, lambda: tick_loop(root))
        root.mainloop()
    except Exception as e:
        print(f"[FATAL] Unhandled exception: {e}", file=sys.stderr)
        sys.exit(1)