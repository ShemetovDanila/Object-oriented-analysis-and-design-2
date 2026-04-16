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

# ─── Настройки ───────────────────────────────────────────────────────────────
HISTORY_SIZE    = 60
TICK_INTERVAL_MS = 1000
LOG_PATH        = "metrics.prom"

BG_DARK   = "#0d0d14"
BG_PANEL  = "#12121c"
BG_CARD   = "#1a1a28"
BG_BORDER = "#252538"
FG_MAIN   = "#e2e8f8"
FG_DIM    = "#7a7fa8"
ACC_BLUE  = "#6eb5ff"
ACC_GREEN = "#5effa0"
ACC_RED   = "#ff6b8a"
ACC_AMBER = "#ffc96b"

CHART_COLORS = {
    "cpu":  "#6eb5ff",
    "ram":  "#5effa0",
    "disk": "#ffc96b",
}

# ─── Паттерн Observer ────────────────────────────────────────────────────────
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
            final = {k: self._overrides.get(k, v) for k, v in raw_metrics.items()}
            snapshot = self._observers[:]
        for obs in snapshot:
            try:
                obs.update(final, timestamp)
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

# ─── Сбор метрик ─────────────────────────────────────────────────────────────
class MetricCollector:
    @staticmethod
    def collect() -> Dict[str, float]:
        try:    cpu = psutil.cpu_percent(interval=0.1)
        except: cpu = 0.0
        try:    ram = psutil.virtual_memory().percent
        except: ram = 0.0
        try:    disk = psutil.disk_usage(os.getcwd()).percent
        except: disk = 0.0
        return {"cpu": cpu, "ram": ram, "disk": disk}

# ─── Логгер Prometheus ───────────────────────────────────────────────────────
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
                print(f"[Logger] IO error: {e}", file=sys.stderr)

    def close(self) -> None:
        if hasattr(self, "_file") and not self._file.closed:
            self._file.close()

# ─── Алерт-правило (ИСПРАВЛЕНО) ──────────────────────────────────────────────
class AlertRuleObserver(Observer):
    def __init__(self, master: tk.Tk, metric: str, threshold: float,
                 message: str, cooldown: int = 5):
        self.master    = master
        self.metric    = metric
        self.threshold = threshold
        self.message   = message
        self.cooldown  = cooldown
        self._last_triggered: float    = 0.0
        self._active_popup: Optional[tk.Toplevel] = None

    def update(self, metrics: Dict[str, float], timestamp: float) -> None:
        val = metrics.get(self.metric)
        if val is None or val < self.threshold:
            return
        if (timestamp - self._last_triggered) < self.cooldown:
            return
        self._last_triggered = timestamp
        # ✅ Передаём в главный поток — безопасно для Tkinter
        try:
            self.master.after(0, self._show_alert, val)
        except Exception:
            pass  # master мог быть уничтожен

    def _show_alert(self, val: float) -> None:
        # ✅ Проверяем, жив ли master
        if not self.master or not self.master.winfo_exists():
            return
        # ✅ Не открываем второй попап, если первый ещё жив
        if self._active_popup is not None and self._active_popup.winfo_exists():
            return

        popup = tk.Toplevel(self.master)
        popup.title("SYSTEM ALERT")
        popup.geometry("420x200")
        popup.configure(bg=BG_PANEL)
        popup.attributes("-topmost", True)
        popup.resizable(False, False)
        self._active_popup = popup

        # Заголовок
        tk.Label(popup, text="⚠  ALERT TRIGGERED",
                 fg=ACC_RED, bg=BG_PANEL,
                 font=("Courier New", 13, "bold")).pack(pady=(18, 4))

        # Сообщение
        tk.Label(popup, text=self.message,
                 fg=FG_MAIN, bg=BG_PANEL,
                 font=("Courier New", 10),
                 wraplength=380, justify="center").pack(pady=4)

        # Метрика
        tk.Label(popup,
                 text=f"{self.metric.upper()}  {val:.1f}%  ≥  threshold {self.threshold:.0f}%",
                 fg=FG_DIM, bg=BG_PANEL,
                 font=("Courier New", 9)).pack(pady=2)

        # Кнопка
        tk.Button(popup, text="ACKNOWLEDGE",
                  bg=ACC_RED, fg=BG_DARK,
                  font=("Courier New", 10, "bold"),
                  relief="flat", bd=0, padx=20, pady=6,
                  activebackground="#ff9aae",
                  cursor="hand2",
                  command=self.dismiss).pack(pady=14)

        # ✅ Закрытие крестиком тоже вызывает dismiss
        popup.protocol("WM_DELETE_WINDOW", self.dismiss)

    def dismiss(self) -> None:
        popup = self._active_popup
        self._active_popup = None
        if popup is not None:
            try:
                if popup.winfo_exists():
                    popup.destroy()
            except Exception:
                pass

# ─── Главный дашборд ─────────────────────────────────────────────────────────
class DashboardGUI(Observer):
    def __init__(self, root: tk.Tk, engine: MetricSubject):
        self.root   = root
        self.engine = engine
        self.history: Dict[str, deque] = {
            m: deque(maxlen=HISTORY_SIZE) for m in ["cpu", "ram", "disk"]
        }
        self.figures:  Dict[str, Figure]             = {}
        self.axes:     Dict[str, object]             = {}
        self.lines:    Dict[str, object]             = {}
        self.canvases: Dict[str, FigureCanvasTkAgg]  = {}
        self.value_labels: Dict[str, tk.Label]       = {}

        # Алерты: id → правило / мета
        self.alert_rules: Dict[int, AlertRuleObserver] = {}
        self.alert_meta:  Dict[int, dict]              = {}
        self.next_alert_id = 1

        self.admin_window   = None
        self.admin_controls = {}

        self._build_ui()

    # ── UI ──────────────────────────────────────────────────────────────────
    def _build_ui(self):
        self.root.title("System Monitor")
        self.root.geometry("1400x860")
        self.root.configure(bg=BG_DARK)
        self.root.minsize(900, 600)

        # Глобальный стиль ttk
        style = ttk.Style()
        style.theme_use("clam")
        style.configure("TFrame",       background=BG_DARK)
        style.configure("TLabel",       background=BG_DARK,   foreground=FG_MAIN, font=("Courier New", 10))
        style.configure("TScrollbar",   background=BG_BORDER, troughcolor=BG_PANEL, arrowcolor=FG_DIM)
        style.configure("Treeview",     background=BG_CARD,   foreground=FG_MAIN,
                        fieldbackground=BG_CARD, rowheight=26, font=("Courier New", 9))
        style.configure("Treeview.Heading", background=BG_BORDER, foreground=ACC_BLUE,
                        font=("Courier New", 9, "bold"), relief="flat")
        style.map("Treeview", background=[("selected", BG_BORDER)],
                  foreground=[("selected", FG_MAIN)])
        style.configure("TCombobox",    fieldbackground=BG_CARD, background=BG_CARD,
                        foreground=FG_MAIN, arrowcolor=ACC_BLUE)
        style.configure("TEntry",       fieldbackground=BG_CARD, foreground=FG_MAIN,
                        insertcolor=FG_MAIN)
        style.configure("TScale",       background=BG_PANEL,  troughcolor=BG_BORDER,
                        sliderthickness=14)
        style.configure("TCheckbutton", background=BG_PANEL,  foreground=FG_MAIN,
                        font=("Courier New", 10))

        # ── Хедер ──
        header = tk.Frame(self.root, bg=BG_PANEL, height=54)
        header.pack(fill="x")
        header.pack_propagate(False)

        tk.Label(header, text="◈ SYSTEM MONITOR",
                 fg=ACC_BLUE, bg=BG_PANEL,
                 font=("Courier New", 15, "bold")).pack(side="left", padx=20)

        self._clock_label = tk.Label(header, text="", fg=FG_DIM, bg=BG_PANEL,
                                     font=("Courier New", 10))
        self._clock_label.pack(side="left", padx=10)

        tk.Button(header, text="ADMIN PANEL",
                  bg=BG_BORDER, fg=ACC_BLUE,
                  font=("Courier New", 9, "bold"),
                  relief="flat", bd=0, padx=14, pady=6,
                  activebackground=BG_CARD, cursor="hand2",
                  command=self._toggle_admin).pack(side="right", padx=20, pady=10)

        self._update_clock()

        # ── Тело ──
        body = tk.Frame(self.root, bg=BG_DARK)
        body.pack(fill="both", expand=True)

        # Левая часть со скроллом
        left_wrap = tk.Frame(body, bg=BG_DARK)
        left_wrap.pack(side="left", fill="both", expand=True)

        left_canvas = tk.Canvas(left_wrap, bg=BG_DARK, highlightthickness=0)
        left_scroll  = ttk.Scrollbar(left_wrap, orient="vertical", command=left_canvas.yview)
        self.scroll_frame = tk.Frame(left_canvas, bg=BG_DARK)

        self.scroll_frame.bind(
            "<Configure>",
            lambda e: left_canvas.configure(scrollregion=left_canvas.bbox("all"))
        )
        left_canvas.configure(yscrollcommand=left_scroll.set)
        left_canvas.pack(side="left", fill="both", expand=True)
        left_scroll.pack(side="left", fill="y")
        left_canvas.create_window((0, 0), window=self.scroll_frame, anchor="nw")

        for btn in ("<MouseWheel>",):
            left_canvas.bind_all(btn, lambda e: left_canvas.yview_scroll(int(-1*(e.delta/120)), "units"))
        left_canvas.bind_all("<Button-4>", lambda e: left_canvas.yview_scroll(-1, "units"))
        left_canvas.bind_all("<Button-5>", lambda e: left_canvas.yview_scroll(1, "units"))

        # Карточки-графики
        CHARTS = [
            ("cpu",  "CPU",  "⬡"),
            ("ram",  "RAM",  "⬡"),
            ("disk", "DISK", "⬡"),
        ]
        for key, label, icon in CHARTS:
            self._build_chart_card(key, label, icon)

        # Правая панель алертов
        right = tk.Frame(body, bg=BG_PANEL, width=390)
        right.pack(side="right", fill="y")
        right.pack_propagate(False)

        self._build_alert_panel(right)

    def _build_chart_card(self, key: str, label: str, icon: str):
        color = CHART_COLORS[key]

        card = tk.Frame(self.scroll_frame, bg=BG_CARD,
                        highlightbackground=BG_BORDER, highlightthickness=1)
        card.pack(fill="x", padx=16, pady=8)

        # Заголовок карточки
        top = tk.Frame(card, bg=BG_CARD)
        top.pack(fill="x", padx=14, pady=(10, 0))

        tk.Label(top, text=f"{icon} {label}",
                 fg=color, bg=BG_CARD,
                 font=("Courier New", 11, "bold")).pack(side="left")

        val_lbl = tk.Label(top, text="—",
                           fg=FG_MAIN, bg=BG_CARD,
                           font=("Courier New", 20, "bold"))
        val_lbl.pack(side="right")
        self.value_labels[key] = val_lbl

        # График
        fig = Figure(figsize=(8, 1.9), dpi=96)
        fig.patch.set_facecolor(BG_CARD)
        ax = fig.add_subplot(111)
        ax.set_facecolor(BG_DARK)
        ax.set_ylim(0, 100)
        ax.set_xlim(0, HISTORY_SIZE)
        ax.set_yticks([0, 25, 50, 75, 100])
        ax.tick_params(colors=FG_DIM, labelsize=7)
        for spine in ax.spines.values():
            spine.set_color(BG_BORDER)
        ax.grid(axis="y", color=BG_BORDER, linewidth=0.5, linestyle="--")

        # Заливка под линией
        fill = ax.fill_between([], [], alpha=0.15, color=color)
        line, = ax.plot([], [], color=color, linewidth=1.8, solid_capstyle="round")

        fig.tight_layout(pad=0.6)

        canvas = FigureCanvasTkAgg(fig, master=card)
        canvas.draw()
        canvas.get_tk_widget().pack(fill="x", padx=6, pady=(4, 8))

        self.figures[key]  = fig
        self.axes[key]     = ax
        self.lines[key]    = line
        self.canvases[key] = canvas
        # Сохраняем fill для перерисовки
        self._fills = getattr(self, "_fills", {})
        self._fills[key] = fill

    def _build_alert_panel(self, parent: tk.Frame):
        tk.Label(parent, text="⚡ ALERT RULES",
                 fg=ACC_AMBER, bg=BG_PANEL,
                 font=("Courier New", 11, "bold")).pack(pady=(16, 6), anchor="w", padx=16)

        # Таблица
        tree_frame = tk.Frame(parent, bg=BG_PANEL)
        tree_frame.pack(fill="both", expand=True, padx=12)

        self.tree = ttk.Treeview(
            tree_frame,
            columns=("id", "metric", "threshold", "message"),
            show="headings", height=18,
            selectmode="browse"
        )
        self.tree.heading("id",        text="#")
        self.tree.heading("metric",    text="METRIC")
        self.tree.heading("threshold", text="THR %")
        self.tree.heading("message",   text="MESSAGE")

        self.tree.column("id",        width=28,  anchor="center", stretch=False)
        self.tree.column("metric",    width=58,  anchor="center", stretch=False)
        self.tree.column("threshold", width=54,  anchor="center", stretch=False)
        self.tree.column("message",   width=220, anchor="w")

        ts = ttk.Scrollbar(tree_frame, orient="vertical", command=self.tree.yview)
        self.tree.configure(yscrollcommand=ts.set)
        self.tree.pack(side="left", fill="both", expand=True)
        ts.pack(side="right", fill="y")

        # Кнопки
        btn_bar = tk.Frame(parent, bg=BG_PANEL)
        btn_bar.pack(fill="x", padx=12, pady=10)

        def _btn(text, color, cmd):
            return tk.Button(btn_bar, text=text,
                             bg=color, fg=BG_DARK,
                             font=("Courier New", 9, "bold"),
                             relief="flat", bd=0, padx=10, pady=6,
                             activebackground=color, cursor="hand2",
                             command=cmd)

        _btn("+ ADD",    ACC_GREEN,  lambda: self._open_alert_dialog()).pack(side="left", fill="x", expand=True, padx=2)
        _btn("EDIT",     ACC_BLUE,   self._edit_selected).pack(side="left", fill="x", expand=True, padx=2)
        _btn("DELETE",   ACC_RED,    self._delete_selected).pack(side="left", fill="x", expand=True, padx=2)

    # ── Часы ────────────────────────────────────────────────────────────────
    def _update_clock(self):
        from datetime import datetime
        self._clock_label.config(text=datetime.now().strftime("%Y-%m-%d  %H:%M:%S"))
        self.root.after(1000, self._update_clock)

    # ── Обновление графиков (вызывается из главного потока через root.after) ──
    def update(self, metrics: Dict[str, float], timestamp: float) -> None:
        self.root.after(0, self._update_plots, metrics)

    def _update_plots(self, metrics: Dict[str, float]) -> None:
        for key, val in metrics.items():
            if key not in self.history:
                continue
            self.history[key].append(val)
            data = list(self.history[key])
            x    = list(range(len(data)))

            ax   = self.axes[key]
            self.lines[key].set_data(x, data)

            # Перерисовываем заливку
            old_fill = self._fills.get(key)
            if old_fill:
                old_fill.remove()
            self._fills[key] = ax.fill_between(x, data,
                                                alpha=0.12,
                                                color=CHART_COLORS[key])

            self.canvases[key].draw_idle()

            # Цифровое значение
            lbl = self.value_labels.get(key)
            if lbl:
                color = (ACC_RED if val >= 85
                         else ACC_AMBER if val >= 60
                         else ACC_GREEN)
                lbl.config(text=f"{val:.1f}%", fg=color)

    # ── Диалог алерта ────────────────────────────────────────────────────────
    def _open_alert_dialog(self, edit_id: Optional[int] = None):
        is_edit = edit_id is not None

        dlg = tk.Toplevel(self.root)
        dlg.title("Edit Alert Rule" if is_edit else "New Alert Rule")
        dlg.geometry("340x300")
        dlg.configure(bg=BG_PANEL)
        dlg.resizable(False, False)
        dlg.transient(self.root)
        dlg.grab_set()

        cur_metric    = self.alert_meta[edit_id]["metric"]    if is_edit else "cpu"
        cur_threshold = str(self.alert_meta[edit_id]["threshold"]) if is_edit else "90"
        cur_message   = self.alert_meta[edit_id]["message"]   if is_edit else ""

        def lbl(text):
            tk.Label(dlg, text=text, fg=FG_DIM, bg=BG_PANEL,
                     font=("Courier New", 9)).pack(anchor="w", padx=30, pady=(10, 0))

        lbl("METRIC")
        metric_var = tk.StringVar(value=cur_metric)
        cb = ttk.Combobox(dlg, textvariable=metric_var,
                          values=["cpu", "ram", "disk"],
                          state="readonly", width=14, justify="center")
        cb.pack(padx=30, anchor="w")

        lbl("THRESHOLD  (%)")
        thr_entry = ttk.Entry(dlg, width=16, justify="center", font=("Courier New", 10))
        thr_entry.insert(0, cur_threshold)
        thr_entry.pack(padx=30, anchor="w")

        lbl("MESSAGE")
        msg_entry = ttk.Entry(dlg, width=36, justify="left", font=("Courier New", 10))
        msg_entry.insert(0, cur_message)
        msg_entry.pack(padx=30, anchor="w")

        err_lbl = tk.Label(dlg, text="", fg=ACC_RED, bg=BG_PANEL, font=("Courier New", 9))
        err_lbl.pack(pady=(6, 0))

        def save():
            raw_thr = thr_entry.get().strip()
            try:
                threshold = float(raw_thr)
            except ValueError:
                err_lbl.config(text="Threshold must be a number (e.g. 80)")
                return

            if not (0 <= threshold <= 100):
                err_lbl.config(text="Threshold must be between 0 and 100")
                return

            metric  = metric_var.get()
            message = msg_entry.get().strip() or f"{metric.upper()} usage exceeded {threshold:.0f}%"

            if is_edit:
                # Отписываем старое правило
                old_rule = self.alert_rules.pop(edit_id)
                self.engine.unsubscribe(old_rule)
                old_rule.dismiss()
                del self.alert_meta[edit_id]

            # Создаём новое
            rule = AlertRuleObserver(self.root, metric, threshold, message, cooldown=5)
            self.engine.subscribe(rule)

            # ✅ При редактировании сохраняем тот же id; при создании — следующий
            alert_id = edit_id if is_edit else self.next_alert_id
            if not is_edit:
                self.next_alert_id += 1

            self.alert_rules[alert_id] = rule
            self.alert_meta[alert_id]  = {
                "metric":    metric,
                "threshold": threshold,
                "message":   message,
            }
            self._refresh_tree()
            dlg.destroy()

        tk.Button(dlg, text="SAVE",
                  bg=ACC_BLUE, fg=BG_DARK,
                  font=("Courier New", 11, "bold"),
                  relief="flat", bd=0, padx=20, pady=7,
                  activebackground="#9ecfff", cursor="hand2",
                  command=save).pack(pady=16)

    def _refresh_tree(self):
        for row in self.tree.get_children():
            self.tree.delete(row)
        for aid, meta in sorted(self.alert_meta.items()):
            msg = meta["message"]
            if len(msg) > 28:
                msg = msg[:25] + "..."
            self.tree.insert("", "end", iid=str(aid),
                             values=(aid, meta["metric"].upper(),
                                     f"{meta['threshold']:.0f}", msg))

    def _edit_selected(self):
        sel = self.tree.selection()
        if not sel:
            messagebox.showwarning("No selection", "Select an alert to edit.", parent=self.root)
            return
        self._open_alert_dialog(edit_id=int(sel[0]))

    def _delete_selected(self):
        sel = self.tree.selection()
        if not sel:
            messagebox.showwarning("No selection", "Select an alert to delete.", parent=self.root)
            return
        if not messagebox.askyesno("Delete", "Remove this alert rule?", parent=self.root):
            return
        aid  = int(sel[0])
        rule = self.alert_rules.pop(aid, None)
        if rule:
            self.engine.unsubscribe(rule)
            rule.dismiss()
        self.alert_meta.pop(aid, None)
        self._refresh_tree()

    # ── Админ-панель ─────────────────────────────────────────────────────────
    def _toggle_admin(self):
        if self.admin_window and self.admin_window.winfo_exists():
            self.admin_window.deiconify()
            self.admin_window.lift()
            return
        win = tk.Toplevel(self.root)
        win.title("Admin Override")
        win.geometry("460x420")
        win.configure(bg=BG_PANEL)
        win.resizable(False, False)
        self.admin_window = win
        self._build_admin_ui(win)

    def _build_admin_ui(self, win: tk.Toplevel):
        tk.Label(win, text="OVERRIDE CONTROLS",
                 fg=ACC_AMBER, bg=BG_PANEL,
                 font=("Courier New", 13, "bold")).pack(pady=16)

        self.admin_controls = {}

        for metric in ["cpu", "ram", "disk"]:
            color = CHART_COLORS[metric]

            frm = tk.Frame(win, bg=BG_CARD,
                           highlightbackground=BG_BORDER, highlightthickness=1)
            frm.pack(fill="x", padx=24, pady=6)

            header = tk.Frame(frm, bg=BG_CARD)
            header.pack(fill="x", padx=12, pady=(10, 4))

            tk.Label(header, text=metric.upper(),
                     fg=color, bg=BG_CARD,
                     font=("Courier New", 10, "bold")).pack(side="left")

            var     = tk.BooleanVar(value=False)
            val_lbl = tk.Label(header, text="—",
                               fg=FG_DIM, bg=BG_CARD,
                               font=("Courier New", 10))
            val_lbl.pack(side="right")

            def _on_scale(v, m=metric, lbl=val_lbl):
                fv = float(v)
                lbl.config(text=f"{fv:.1f}%")
                if self.admin_controls[m]["var"].get():
                    self.engine.set_override(m, fv)

            scale = ttk.Scale(frm, from_=0, to=100, orient="horizontal",
                              length=380, command=_on_scale)
            scale.pack(padx=12, pady=(0, 4))

            def _on_check(m=metric):
                if self.admin_controls[m]["var"].get():
                    self.engine.set_override(m, self.admin_controls[m]["scale"].get())
                else:
                    self.engine.clear_override(m)

            ttk.Checkbutton(frm, text="Enable override",
                            variable=var, command=_on_check).pack(padx=12, pady=(0, 10), anchor="w")

            self.admin_controls[metric] = {"var": var, "scale": scale}

        # Синхронизируем с текущими оверрайдами
        overrides = self.engine.get_overrides()
        for m, ctrl in self.admin_controls.items():
            if m in overrides:
                ctrl["var"].set(True)
                ctrl["scale"].set(overrides[m])

        tk.Button(win, text="CLEAR ALL OVERRIDES",
                  bg=ACC_RED, fg=BG_DARK,
                  font=("Courier New", 9, "bold"),
                  relief="flat", bd=0, padx=14, pady=6,
                  cursor="hand2",
                  command=self._clear_all_overrides).pack(pady=14)

    def _clear_all_overrides(self):
        for m, ctrl in self.admin_controls.items():
            ctrl["var"].set(False)
            self.engine.clear_override(m)

    # ── Очистка при закрытии ─────────────────────────────────────────────────
    def cleanup(self):
        for rule in list(self.alert_rules.values()):
            rule.dismiss()
            self.engine.unsubscribe(rule)
        self.alert_rules.clear()
        self.alert_meta.clear()
        if self.admin_window and self.admin_window.winfo_exists():
            self.admin_window.destroy()

# ─── Оркестратор ─────────────────────────────────────────────────────────────
class MonitorOrchestrator:
    def __init__(self, collector: MetricCollector = None):
        self.root      = tk.Tk()
        self.engine    = MetricSubject()
        self.collector = collector or MetricCollector()
        self.logger    = PrometheusLogger(LOG_PATH, host="monitor-01")
        self.dashboard = DashboardGUI(self.root, self.engine)
        self.engine.subscribe(self.logger)
        self.engine.subscribe(self.dashboard)
        self.root.protocol("WM_DELETE_WINDOW", self._shutdown)
        self.root.after(TICK_INTERVAL_MS, self._tick_loop)

    def _tick_loop(self):
        try:
            raw = self.collector.collect()
            self.engine.notify(raw, time.time())
        except Exception as e:
            print(f"[Tick Error] {e}", file=sys.stderr)
        finally:
            self.root.after(TICK_INTERVAL_MS, self._tick_loop)

    def _shutdown(self):
        self.dashboard.cleanup()
        self.engine.unsubscribe(self.dashboard)
        self.engine.unsubscribe(self.logger)
        self.logger.close()
        self.root.destroy()

# ─── Точка входа ─────────────────────────────────────────────────────────────
if __name__ == "__main__":
    try:
        app = MonitorOrchestrator()
        app.root.mainloop()
    except Exception as e:
        print(f"[FATAL] {e}", file=sys.stderr)
        sys.exit(1)