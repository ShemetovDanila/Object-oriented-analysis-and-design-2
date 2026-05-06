package com.gallery;
import com.gallery.model.*;
import com.gallery.service.Gallery;
import javax.swing.*;
import java.awt.*;
import java.util.List;

public class App extends JFrame {
    private final JList<Preview> list = new JList<>();
    private final JTextArea details = new JTextArea(10, 40);
    private final JLabel status = new JLabel("Готово");

    public App() {
        super("🖼️ Lazy Load Demo");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout(10, 10));

        // Визуальное оформление элементов списка
        list.setCellRenderer((lst, val, idx, sel, focus) -> {
            JLabel lbl = new JLabel(val.title() + "  [" + val.thumbnail() + "]");
            lbl.setOpaque(true);
            lbl.setBackground(sel ? Color.LIGHT_GRAY : Color.WHITE);
            return lbl;
        });

        // При клике → асинхронная подгрузка деталей
        list.addListSelectionListener(e -> {
            if (!e.getValueIsAdjusting() && list.getSelectedValue() != null) {
                loadDetails(list.getSelectedValue().id());
            }
        });
        add(new JScrollPane(list), BorderLayout.WEST);

        details.setEditable(false);
        details.setFont(new Font("Monospaced", Font.PLAIN, 12));
        add(new JScrollPane(details), BorderLayout.CENTER);

        status.setBorder(BorderFactory.createEmptyBorder(5, 10, 5, 10));
        add(status, BorderLayout.SOUTH);

        setSize(900, 600);
        setLocationRelativeTo(null);
    }

    // Асинхронная загрузка деталей (не блокирует UI)
    private void loadDetails(Long id) {
        new SwingWorker<Item, Void>() {
            protected Item doInBackground() { status.setText("Загрузка..."); return Gallery.loadDetails(id); }
            protected void done() {
                try {
                    Item i = get();
                    details.setText("📌 " + i.title() + "\n\n🖼️ " + i.thumbnail() + "\n\n📝 " + i.description() + "\n\n🏷️ " + i.tags());
                    status.setText("Загружено");
                } catch (Exception ex) { details.setText("Ошибка: " + ex.getMessage()); }
            }
        }.execute();
    }

    // Загрузка списка в режиме EAGER
    private void loadListEager() {
        new SwingWorker<List<Item>, Void>() {
            protected List<Item> doInBackground() { status.setText("Загрузка ВСЕХ данных (EAGER)..."); return Gallery.loadAllEager(); }
            protected void done() {
                try {
                    list.setListData(get().stream().map(i -> new Preview(i.id(), i.title(), i.thumbnail())).toArray(Preview[]::new));
                    status.setText("Загружено " + list.getModel().getSize() + " эл. (все данные в памяти)");
                } catch (Exception ex) { JOptionPane.showMessageDialog(App.this, "Ошибка: " + ex.getMessage()); }
            }
        }.execute();
    }

    // Загрузка списка в режиме LAZY
    private void loadListLazy() {
        new SwingWorker<List<Preview>, Void>() {
            protected List<Preview> doInBackground() { status.setText("Загрузка превью (LAZY)..."); return Gallery.loadPreviews(); }
            protected void done() {
                try {
                    list.setListData(get().toArray(new Preview[0]));
                    status.setText("Загружены превью. Детали — по клику.");
                } catch (Exception ex) { JOptionPane.showMessageDialog(App.this, "Ошибка: " + ex.getMessage()); }
            }
        }.execute();
    }

    // Меню выбора режима загрузки
    private JMenuBar createMenu() {
        JMenuBar bar = new JMenuBar();
        JMenu mode = new JMenu("🔄 Режим загрузки");
        JMenuItem eager = new JMenuItem("❌ EAGER (грузить всё)");
        JMenuItem lazy = new JMenuItem("✅ LAZY (грузить по требованию)");
        eager.addActionListener(e -> loadListEager());
        lazy.addActionListener(e -> loadListLazy());
        mode.add(eager); mode.add(lazy);
        bar.add(mode);
        return bar;
    }

    // ТОЧКА ВХОДА
    public static void main(String[] args) throws Exception {
        DB.init(); // Инициализация БД
        SwingUtilities.invokeLater(() -> {
            App app = new App();
            app.setJMenuBar(app.createMenu());
            app.setVisible(true);
            JOptionPane.showMessageDialog(app,
                "🔴 Меню → EAGER: загрузка с задержкой (все данные сразу).\n" +
                "🟢 Меню → LAZY: мгновенный список, детали подгружаются по клику.\n" +
                "Смотрите время выполнения в консоли!", "Lazy Load Demo", JOptionPane.INFORMATION_MESSAGE);
        });
    }
}