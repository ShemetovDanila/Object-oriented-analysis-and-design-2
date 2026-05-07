package com.gallery;

import com.gallery.db.DB;
import com.gallery.model.*;
import com.gallery.service.CatalogService;
import com.gallery.ui.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;
import java.awt.event.*;
import java.util.List;

/**
 * Главное окно приложения.
 *
 * Демонстрирует паттерн «Загрузка по требованию» (Lazy Load):
 *  — LAZY-режим:  при открытии каталога грузятся только базовые поля;
 *                  подробности загружаются только при клике на товар.
 *  — EAGER-режим: все поля всех товаров загружаются сразу.
 */
public class App extends JFrame {

    private boolean lazyMode = true;  // текущий режим

    private final ProductListPanel   listPanel   = new ProductListPanel();
    private final ProductDetailPanel detailPanel = new ProductDetailPanel();
    private final ModeHeaderPanel    modeHeader  = new ModeHeaderPanel();
    private final StatusBar          statusBar   = new StatusBar();

    // Меню-переключатели
    private JRadioButtonMenuItem menuLazy;
    private JRadioButtonMenuItem menuEager;

    public App() {
        super("Каталог товаров — демонстрация паттерна Lazy Load");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setMinimumSize(new Dimension(900, 600));
        setSize(1100, 720);
        setLocationRelativeTo(null);

        buildUI();
        loadCatalog();
    }

    // ─────────────────────── Построение UI ──────────────────────

    private void buildUI() {
        setLayout(new BorderLayout());

        // Шапка окна
        add(buildHeaderBar(), BorderLayout.NORTH);

        // Основная область
        JSplitPane split = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, listPanel, detailPanel);
        split.setDividerLocation(290);
        split.setDividerSize(1);
        split.setBorder(null);

        JPanel center = new JPanel(new BorderLayout());
        center.add(modeHeader, BorderLayout.NORTH);
        center.add(split, BorderLayout.CENTER);

        add(center, BorderLayout.CENTER);
        add(statusBar, BorderLayout.SOUTH);

        setJMenuBar(buildMenuBar());

        // При выборе товара в списке
        listPanel.setOnSelect(card -> {
            if (lazyMode) {
                loadDetailsLazy(card);
            } else {
                // В Eager-режиме детали уже в памяти — берём из кэша
                showEagerDetail(card);
            }
        });
    }

    /**
     * Верхняя панель с логотипом и кнопкой загрузки.
     */
    private JPanel buildHeaderBar() {
        JPanel bar = new JPanel(new BorderLayout(16, 0));
        bar.setBackground(AppTheme.BG_HEADER);
        bar.setBorder(new EmptyBorder(14, 20, 14, 20));

        // Левая часть — логотип
        JLabel logo = new JLabel("ShopDemo");
        logo.setFont(new Font("Segoe UI Semibold", Font.PLAIN, 18));
        logo.setForeground(AppTheme.TEXT_ON_DARK);

        JLabel subtitle = new JLabel("Паттерн: Загрузка по требованию");
        subtitle.setFont(AppTheme.FONT_SMALL);
        subtitle.setForeground(new Color(0x9A94C8));

        JPanel logoBlock = new JPanel();
        logoBlock.setLayout(new BoxLayout(logoBlock, BoxLayout.Y_AXIS));
        logoBlock.setOpaque(false);
        logoBlock.add(logo);
        logoBlock.add(Box.createVerticalStrut(2));
        logoBlock.add(subtitle);

        // Правая часть — кнопка загрузки
        JButton loadBtn = new JButton("Загрузить каталог");
        loadBtn.setFont(AppTheme.FONT_BODY);
        loadBtn.setForeground(AppTheme.TEXT_ON_DARK);
        loadBtn.setBackground(AppTheme.ACCENT_LAZY);
        loadBtn.setBorder(new CompoundBorder(
                new LineBorder(new Color(0x7F77DD), 1, true),
                new EmptyBorder(8, 16, 8, 16)));
        loadBtn.setFocusPainted(false);
        loadBtn.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        loadBtn.addActionListener(e -> loadCatalog());
        loadBtn.addMouseListener(new MouseAdapter() {
            public void mouseEntered(MouseEvent e) { loadBtn.setBackground(new Color(0x3C3489)); }
            public void mouseExited(MouseEvent e)  { loadBtn.setBackground(AppTheme.ACCENT_LAZY); }
        });

        bar.add(logoBlock, BorderLayout.WEST);
        bar.add(loadBtn,   BorderLayout.EAST);

        return bar;
    }

    /**
     * Меню-бар с переключателем режима.
     */
    private JMenuBar buildMenuBar() {
    JMenuBar mb = new JMenuBar();
    mb.setBackground(AppTheme.BG_SIDEBAR);
    mb.setBorder(new MatteBorder(0, 0, 1, 0, AppTheme.BORDER));

    // ── Меню «Режим загрузки» ────────────────────────────────
    JMenu modeMenu = new JMenu("Режим загрузки");
    modeMenu.setFont(AppTheme.FONT_BODY);
    
    ButtonGroup group = new ButtonGroup();
    
    menuLazy = new JRadioButtonMenuItem("С паттерном «Загрузка по требованию» (Lazy)", true);
    menuLazy.setFont(AppTheme.FONT_BODY);
    menuLazy.setToolTipText("Базовые поля грузятся сразу. Описания — только при клике.");
    menuLazy.addActionListener(e -> switchMode(true));
    
    menuEager = new JRadioButtonMenuItem("Без паттерна — загрузить всё сразу (Eager)");
    menuEager.setFont(AppTheme.FONT_BODY);
    menuEager.setToolTipText("Все поля всех товаров загружаются при старте.");
    menuEager.addActionListener(e -> switchMode(false));
    
    group.add(menuLazy);
    group.add(menuEager);
    modeMenu.add(menuLazy);
    modeMenu.add(menuEager);
    modeMenu.addSeparator();
    
    JMenuItem reloadItem = new JMenuItem("Перезагрузить каталог");
    reloadItem.setFont(AppTheme.FONT_BODY);
    reloadItem.addActionListener(e -> loadCatalog());
    modeMenu.add(reloadItem);
    
    mb.add(modeMenu);

    // ── Меню «О паттерне» ────────────────────────────────────
    JMenu infoMenu = new JMenu("О паттерне");
    infoMenu.setFont(AppTheme.FONT_BODY);
    
    JMenuItem aboutItem = new JMenuItem("Как это работает?");
    aboutItem.setFont(AppTheme.FONT_BODY);
    aboutItem.addActionListener(e -> showAboutDialog());
    infoMenu.add(aboutItem);
    
    mb.add(infoMenu);
    return mb;
}

    // ─────────────────────── Логика загрузки ────────────────────

    // Кэш полных товаров для Eager-режима
    private List<Product> eagerCache = null;

    /**
     * Загружает каталог согласно текущему режиму.
     */
    private void loadCatalog() {
        eagerCache = null;
        listPanel.clearSelection();
        detailPanel.showEmpty();
        statusBar.setLoading(true);

        if (lazyMode) {
            // LAZY: грузим только карточки (лёгкие поля)
            statusBar.setStatus("Загрузка списка товаров (только базовые поля)...");
            new SwingWorker<CatalogService.LoadResult<List<ProductCard>>, Void>() {
                protected CatalogService.LoadResult<List<ProductCard>> doInBackground() {
                    return CatalogService.loadCards();
                }

                protected void done() {
                    try {
                        CatalogService.LoadResult<List<ProductCard>> r = get();
                        listPanel.setProducts(r.data());
                        statusBar.setLoading(false);
                        statusBar.setStatus("Каталог загружен. Выберите товар для просмотра деталей.");
                        statusBar.setStats(r.data().size() + " товаров  •  " +
                                r.elapsedMs() + " мс  •  ~" +
                                (r.bytesEstimate() / 1024) + " КБ");
                    } catch (Exception ex) {
                        handleError(ex);
                    }
                }
            }.execute();

        } else {
            // EAGER: грузим ВСЁ сразу
            statusBar.setStatus("Загрузка ВСЕХ товаров со всеми полями...");
            new SwingWorker<CatalogService.LoadResult<List<Product>>, Void>() {
                protected CatalogService.LoadResult<List<Product>> doInBackground() {
                    return CatalogService.loadAll();
                }

                protected void done() {
                    try {
                        CatalogService.LoadResult<List<Product>> r = get();
                        eagerCache = r.data();
                        // Конвертируем в карточки для отображения
                        List<ProductCard> cards = r.data().stream()
                                .map(p -> new ProductCard(p.id(), p.name(), p.price(), p.category()))
                                .toList();
                        listPanel.setProducts(cards);
                        statusBar.setLoading(false);
                        statusBar.setStatus("Все товары загружены (включая описания и характеристики).");
                        statusBar.setStats(r.data().size() + " товаров  •  " +
                                r.elapsedMs() + " мс  •  ~" +
                                (r.bytesEstimate() / 1024) + " КБ загружено в память");
                    } catch (Exception ex) {
                        handleError(ex);
                    }
                }
            }.execute();
        }
    }

    /**
     * [LAZY] Загружает детали товара по клику — только одного.
     */
    private void loadDetailsLazy(ProductCard card) {
        detailPanel.showLoading();
        statusBar.setStatus("Загрузка деталей: " + card.name() + "...");
        statusBar.setLoading(true);

        new SwingWorker<CatalogService.LoadResult<Product>, Void>() {
            protected CatalogService.LoadResult<Product> doInBackground() {
                return CatalogService.loadDetails(card.id());
            }

            protected void done() {
                try {
                    CatalogService.LoadResult<Product> r = get();
                    if (r.data() != null) {
                        detailPanel.showProduct(r.data(), r.elapsedMs(), r.bytesEstimate());
                        statusBar.setStatus("Детали загружены за " + r.elapsedMs() + " мс.");
                    }
                    statusBar.setLoading(false);
                } catch (Exception ex) {
                    handleError(ex);
                }
            }
        }.execute();
    }

    /**
     * [EAGER] Показывает детали из кэша — без доп. запроса к БД.
     */
    private void showEagerDetail(ProductCard card) {
        if (eagerCache == null) return;
        eagerCache.stream()
                .filter(p -> p.id().equals(card.id()))
                .findFirst()
                .ifPresent(p -> {
                    detailPanel.showProductEager(p);
                    statusBar.setStatus("Данные взяты из памяти (загружены заранее).");
                });
    }

    /**
     * Переключение режима.
     */
    private void switchMode(boolean lazy) {
        if (this.lazyMode == lazy) return;
        this.lazyMode = lazy;

        if (lazy) {
            modeHeader.showLazy();
            statusBar.setLazyMode();
        } else {
            modeHeader.showEager();
            statusBar.setEagerMode();
        }

        // Перезагружаем каталог в новом режиме
        loadCatalog();
    }

    private void handleError(Exception ex) {
        statusBar.setLoading(false);
        statusBar.setStatus("Ошибка: " + ex.getMessage());
        ex.printStackTrace();
        JOptionPane.showMessageDialog(this,
                "Произошла ошибка:\n" + ex.getMessage(),
                "Ошибка", JOptionPane.ERROR_MESSAGE);
    }

    private void showAboutDialog() {
        String msg = """
                <html>
                <h2 style='color:#3C3489'>Паттерн «Загрузка по требованию» (Lazy Load)</h2>
                <p><b>Суть:</b> загружаем только то, что нужно прямо сейчас.</p>
                <hr>
                <p><b>⚡ Режим LAZY (с паттерном):</b><br>
                • При открытии каталога загружаются только<br>
                &nbsp;&nbsp;<i>лёгкие поля</i>: id, название, цена, категория<br>
                • Описание, характеристики и теги загружаются<br>
                &nbsp;&nbsp;<i>только при клике на товар</i><br>
                • Экономия: не тратим память и время на данные,<br>
                &nbsp;&nbsp;которые пользователь может никогда не открыть</p>
                <hr>
                <p><b>🔥 Режим EAGER (без паттерна):</b><br>
                • Загружаются <i>все поля всех товаров</i> сразу<br>
                • Описания грузятся даже если их никто не откроет<br>
                • Больше памяти, дольше начальная загрузка</p>
                <hr>
                <p style='color:#6B6880; font-size:11px'>
                Используйте меню «⚙ Режим загрузки» для переключения.<br>
                Следите за временем и объёмом загрузки в строке состояния.
                </p>
                </html>
                """;

        JLabel label = new JLabel(msg);
        label.setBorder(new EmptyBorder(8, 8, 8, 8));
        JOptionPane.showMessageDialog(this, label, "О паттерне Lazy Load",
                JOptionPane.INFORMATION_MESSAGE);
    }

    // ──────────────────────── main ───────────────────────────────

    public static void main(String[] args) {
        // Nimbus Look & Feel с кастомными цветами
        try {
            for (UIManager.LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
            UIManager.put("nimbusBase", new Color(0x4A3DB7));
            UIManager.put("control", new Color(0xF8F7F4));
            UIManager.put("text", new Color(0x1E1B2E));
            UIManager.put("nimbusLightBackground", Color.WHITE);
            UIManager.put("nimbusSelectionBackground", new Color(0x4A3DB7));
            UIManager.put("nimbusSelectedText", Color.WHITE);
        } catch (Exception ignored) {}

        try {
            DB.initializeDatabase();
        } catch (Exception e) {
            JOptionPane.showMessageDialog(null,
                    "Ошибка инициализации БД:\n" + e.getMessage(),
                    "Ошибка", JOptionPane.ERROR_MESSAGE);
            System.exit(1);
        }

        SwingUtilities.invokeLater(() -> new App().setVisible(true));
    }
}
