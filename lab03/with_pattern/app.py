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
from abc import ABC, abstractmethod
from typing import Dict, List, Optional

# настройки
HISTORY_SIZE = 60
TICK_INTERVAL_MS = 1000
LOG_PATH = "metrics.prom"
MAX_ALERTS = 50

# интерфейс и ядро рассылки
class Observer(ABC):
    @abstractmethod
    def update(self, metrics: Dict[str, float], timestamp: float) -> None:
        pass

class MetricSubject:
    def __init__(self):
        self._observers: List[Observer] = []
        self._overrides: Dict[str, float] = {}
        self._lock = threading.RLock()

    def subscribe(self, observer: Observer) -> None:
        with self._lock:
            if observer not in self._observers:
                self._observers.append(observer)

    def unsubscribe(self, observer: Observer) -> None:
        with self._lock:
            if observer in self._observers:
                self._observers.remove(observer)

    def notify(self, raw_metrics: Dict[str, float], timestamp: float) -> None:
        with self._lock:
            final_metrics = {k: self._overrides.get(k, v) for k, v in raw_metrics.items()}
            snapshot = self._observers[:]
        
        for obs in snapshot:
            try:
                obs.update(final_metrics, timestamp)
            except Exception as e:
                print(f"[Observer Error] {type(obs).__name__}: {e}", file=sys.stderr)

    def set_override(self, key: str, value: float) -> None:
        with self._lock:
            self._overrides[key] = value

    def clear_override(self, key: str) -> None:
        with self._lock:
            self._overrides.pop(key, None)

    def get_overrides(self) -> Dict[str, float]:
        with self._lock:
            return self._overrides.copy()

# сбор метрик системы
class MetricCollector:
    @staticmethod
    def collect() -> Dict[str, float]:
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

# наблюдатель: логгер в prometheus-формате
class PrometheusLogger(Observer):
    def __init__(self, path: str, host: str = "localhost"):
        self._host = host
        self._lock = threading.Lock()
        try:
            self._file = open(path, "a", encoding="utf-8")
        except OSError as e:
            raise RuntimeError(f"Cannot open log file {path}: {e}")

    def update(self, metrics: Dict[str, float], timestamp: float) -> None:
        with self._lock:
            try:
                ts_ms = int(timestamp * 1000)
                for name, val in metrics.items():
                    self._file.write(f'system_{name}{{host="{self._host}"}} {val:.2f} {ts_ms}\n')
                self._file.flush()
            except OSError as e:
                print(f"[Logger] Disk/IO error: {e}", file=sys.stderr)

    def close(self) -> None:
        if hasattr(self, "_file") and not self._file.closed:
            self._file.close()

# наблюдатель: правило алертинга
class AlertRuleObserver(Observer):
    def __init__(self, master: tk.Tk, metric: str, threshold: float, message: str, cooldown: int = 30):
        self.master = master
        self.metric = metric
        self.threshold = threshold
        self.message = message
        self.cooldown = cooldown
        self._last_triggered = 0.0
        self._active_popup: Optional[tk.Toplevel] = None

    def update(self, metrics: Dict[str, float], timestamp: float) -> None:
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
        tk.Button(popup, text="ACKNOWLEDGE", bg="#f38ba8", fg="#11111b", font=("Segoe UI", 10, "bold"), relief="flat",
                  command=self.dismiss).pack(pady=10)

        popup.protocol("WM_DELETE_WINDOW", self.dismiss)

    def dismiss(self):
        if self._active_popup and self._active_popup.winfo_exists():
            self._active_popup.destroy()
        self._active_popup = None

# наблюдатель: графический дашборд
class DashboardGUI(Observer):
    def __init__(self, root: tk.Tk, engine: MetricSubject):
        self.root = root
        self.engine = engine
        self.history: Dict[str, deque] = {m: deque(maxlen=HISTORY_SIZE) for m in ["cpu", "ram", "disk"]}
        self.figures, self.axes, self.lines, self.canvases = {}, {}, {}, {}
        self._active_alerts: List[AlertRuleObserver] = []
        self._setup_ui()

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

            tk.Button(frame, text="🔔 Configure Alert", bg="#313244", fg="#cdd6f4", font=("Segoe UI", 10), relief="flat",
                      cursor="hand2", command=lambda k=key: self._open_alert_dialog(k)).pack(pady=4, anchor="e")

            self.figures[key] = fig
            self.axes[key] = ax
            self.lines[key] = line
            self.canvases[key] = canvas

    def update(self, metrics: Dict[str, float], timestamp: float) -> None:
        self.root.after(0, self._update_plots, metrics)

    def _update_plots(self, metrics: Dict[str, float]) -> None:
        for key, val in metrics.items():
            if key not in self.history:
                continue
            self.history[key].append(val)
            x = list(range(len(self.history[key])))
            self.lines[key].set_data(x, self.history[key])
            self.axes[key].set_xlim(0, HISTORY_SIZE)
            self.axes[key].set_ylim(0, 100)
            self.canvases[key].draw_idle()

    def _open_alert_dialog(self, metric: str) -> None:
        remaining = MAX_ALERTS - len(self._active_alerts)
        if remaining <= 0:
            messagebox.showwarning("Limit Reached", f"Maximum {MAX_ALERTS} alert rules reached.", parent=self.root)
            return

        dlg = tk.Toplevel(self.root)
        dlg.title(f"⚙️ Alert Rule: {metric.upper()} ({remaining} left)")
        dlg.geometry("320x250")
        dlg.configure(bg="#181825")
        dlg.transient(self.root)
        dlg.grab_set()

        tk.Label(dlg, text="Threshold (%)", fg="#cdd6f4", bg="#181825",
                 font=("Segoe UI", 11)).pack(pady=(15, 0))
        thr_entry = ttk.Entry(dlg, width=10, justify="center")
        thr_entry.insert(0, "90")
        thr_entry.pack(pady=5)

        tk.Label(dlg, text="Alert Message", fg="#cdd6f4", bg="#181825",
                 font=("Segoe UI", 11)).pack(pady=(10, 0))
        msg_entry = ttk.Entry(dlg, width=35, justify="center")
        msg_entry.insert(0, f"⚠️ {metric.upper()} usage exceeded safe limit!")
        msg_entry.pack(pady=5)

        def save():
            try:
                threshold = float(thr_entry.get())
                message = msg_entry.get().strip() or f"{metric} alert"
                rule = AlertRuleObserver(self.root, metric, threshold, message, cooldown=30)
                self.engine.subscribe(rule)
                self._active_alerts.append(rule)
                messagebox.showinfo("✅ Success", f"Alert for {metric} activated.", parent=dlg)
                dlg.destroy()
            except ValueError:
                messagebox.showerror("❌ Error", "Threshold must be a number.", parent=dlg)

        tk.Button(dlg, text="💾 Save & Subscribe", bg="#89b4fa", fg="#11111b", font=("Segoe UI", 11, "bold"),
                  relief="flat", command=save).pack(pady=15)

    def cleanup(self):
        for rule in self._active_alerts:
            rule.dismiss()
            self.engine.unsubscribe(rule)
        self._active_alerts.clear()

# контроллер: админ-панель
class AdminPanel:
    def __init__(self, engine: MetricSubject, parent: tk.Tk):
        self.win = tk.Toplevel(parent)
        self.win.title("🛠️ Admin Override Console")
        self.win.geometry("440x400")
        self.win.configure(bg="#181825")
        self.engine = engine

        style = ttk.Style()
        style.configure("Admin.TFrame", background="#181825")
        style.configure("Admin.TLabel", background="#181825", foreground="#cdd6f4", font=("Segoe UI", 11))
        style.configure("Admin.TCheckbutton", background="#181825", foreground="#cdd6f4")
        style.configure("Admin.TScale", background="#181825", troughcolor="#313244", slidercolor="#89b4fa")
        style.configure("Admin.TButton", background="#313244", foreground="#cdd6f4")

        tk.Label(self.win, text="OVERRIDE CONTROLS", font=("Segoe UI", 14, "bold"),
                 fg="#cdd6f4", bg="#181825").pack(pady=10)

        self.controls = {}
        for metric in ["cpu", "ram", "disk"]:
            frame = ttk.LabelFrame(self.win, text=metric.upper(), padding=10)
            frame.pack(fill="x", padx=20, pady=5)

            var = tk.BooleanVar(value=False)
            ttk.Checkbutton(frame, text="⚡ Enable Override", variable=var,
                            command=lambda m=metric: self._sync(m)).pack(anchor="w")

            scale = ttk.Scale(frame, from_=0, to=100, orient="horizontal", length=340,
                              command=lambda v, m=metric: self._on_slide(m, v))
            scale.pack(pady=5)

            self.controls[metric] = {"var": var, "scale": scale}

        tk.Button(self.win, text="🔄 Sync UI from Engine", bg="#313244", fg="#cdd6f4", font=("Segoe UI", 10),
                  relief="flat", command=self._sync_all_ui).pack(pady=8)

        self._sync_all_ui()

    def _on_slide(self, metric: str, val: str) -> None:
        if self.controls[metric]["var"].get():
            self.engine.set_override(metric, float(val))

    def _sync(self, metric: str) -> None:
        if self.controls[metric]["var"].get():
            self.engine.set_override(metric, self.controls[metric]["scale"].get())
        else:
            self.engine.clear_override(metric)

    def _sync_all_ui(self):
        overrides = self.engine.get_overrides()
        for metric, ctrl in self.controls.items():
            if metric in overrides:
                ctrl["var"].set(True)
                ctrl["scale"].set(overrides[metric])
            else:
                ctrl["var"].set(False)

# оркестратор и цикл обновления
class MonitorOrchestrator:
    def __init__(self, collector: MetricCollector = None):
        self.root = tk.Tk()
        self.engine = MetricSubject()
        self.collector = collector or MetricCollector()
        self.logger = PrometheusLogger(LOG_PATH, host="monitor-01")

        self.dashboard = DashboardGUI(self.root, self.engine)
        self.admin = AdminPanel(self.engine, self.root)

        self.engine.subscribe(self.logger)
        self.engine.subscribe(self.dashboard)

        self.root.protocol("WM_DELETE_WINDOW", self._shutdown)
        self.root.after(TICK_INTERVAL_MS, self._tick_loop)

    def _tick_loop(self):
        try:
            self._tick()
        except Exception as e:
            print(f"[Tick Error] {e}", file=sys.stderr)
        finally:
            self.root.after(TICK_INTERVAL_MS, self._tick_loop)

    def _tick(self):
        raw = self.collector.collect()
        self.engine.notify(raw, time.time())

    def _shutdown(self):
        self.dashboard.cleanup()
        self.engine.unsubscribe(self.dashboard)
        self.engine.unsubscribe(self.logger)
        self.logger.close()
        self.root.destroy()

# точка входа
if __name__ == "__main__":
    try:
        app = MonitorOrchestrator()
        app.root.mainloop()
    except Exception as e:
        print(f"[FATAL] Unhandled exception: {e}", file=sys.stderr)
        sys.exit(1)