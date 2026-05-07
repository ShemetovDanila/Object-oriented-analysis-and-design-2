package com.gallery.ui;

import com.gallery.model.ProductCard;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;

public class ProductListPanel extends JPanel {
    private final DefaultListModel<ProductCard> listModel = new DefaultListModel<>();
    private final JList<ProductCard> jList = new JList<>(listModel);
    private final JTextField searchField;
    private final JLabel countLabel;
    private List<ProductCard> allCards = new ArrayList<>();
    private Consumer<ProductCard> onSelect;

    public ProductListPanel() {
        setLayout(new BorderLayout());
        setBackground(AppTheme.BG_SIDEBAR);
        setBorder(new MatteBorder(0, 0, 0, 1, AppTheme.BORDER));
        setPreferredSize(new Dimension(280, 0));

        // Шапка
        JPanel header = new JPanel(new BorderLayout(0, 8));
        header.setBackground(AppTheme.BG_SIDEBAR);
        header.setBorder(new EmptyBorder(16, 16, 10, 16));

        JLabel title = new JLabel("Каталог товаров");
        title.setFont(AppTheme.FONT_HEADER);
        title.setForeground(AppTheme.TEXT_PRIMARY);

        countLabel = new JLabel("0 товаров");
        countLabel.setFont(AppTheme.FONT_SMALL);
        countLabel.setForeground(AppTheme.TEXT_MUTED);

        JPanel titleRow = new JPanel(new BorderLayout());
        titleRow.setOpaque(false);
        titleRow.add(title, BorderLayout.WEST);
        titleRow.add(countLabel, BorderLayout.EAST);

        // Поиск
        searchField = new JTextField();
        searchField.setFont(AppTheme.FONT_BODY);
        searchField.setForeground(AppTheme.TEXT_PRIMARY);
        searchField.setBackground(AppTheme.BG_PRIMARY);
        searchField.setBorder(BorderFactory.createCompoundBorder(
                new LineBorder(AppTheme.BORDER, 1, true),
                new EmptyBorder(6, 10, 6, 10)));
        searchField.putClientProperty("JTextField.placeholderText", "Поиск...");

        searchField.addKeyListener(new KeyAdapter() {
            @Override public void keyReleased(KeyEvent e) { filterList(searchField.getText()); }
        });

        header.add(titleRow, BorderLayout.NORTH);
        header.add(searchField, BorderLayout.SOUTH);
        add(header, BorderLayout.NORTH);

        // Список
        jList.setCellRenderer(new ProductCellRenderer());
        jList.setBackground(AppTheme.BG_SIDEBAR);
        jList.setSelectionBackground(new Color(0, 0, 0, 0));
        jList.setSelectionForeground(AppTheme.TEXT_PRIMARY);
        jList.setFixedCellHeight(95); // Высота карточки
        jList.setFocusable(true);
        
        jList.addListSelectionListener(e -> {
            if (!e.getValueIsAdjusting() && onSelect != null) {
                ProductCard sel = jList.getSelectedValue();
                if (sel != null) onSelect.accept(sel);
            }
        });

        // Скролл с единым стилем
        JScrollPane scroll = new JScrollPane(jList);
        scroll.setBorder(null);
        scroll.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
        
        // Вызываем утилиту для стилизации
        UIUtils.applyScrollBarStyle(scroll, new Color(0xC8C5D8), AppTheme.BG_SIDEBAR);
        
        add(scroll, BorderLayout.CENTER);
    }

    public void setProducts(List<ProductCard> cards) {
        this.allCards = new ArrayList<>(cards);
        filterList(searchField.getText());
    }

    public void setOnSelect(Consumer<ProductCard> handler) { this.onSelect = handler; }
    public void clearSelection() { jList.clearSelection(); }

    private void filterList(String query) {
        String q = query == null ? "" : query.trim().toLowerCase();
        listModel.clear();
        for (ProductCard c : allCards) {
            if (q.isEmpty() || c.name().toLowerCase().contains(q) || c.category().toLowerCase().contains(q)) {
                listModel.addElement(c);
            }
        }
        countLabel.setText(listModel.getSize() + " товаров");
    }

    // Рендерер карточек
    private static class ProductCellRenderer implements ListCellRenderer<ProductCard> {
        private static final Map<String, Color> CAT_COLORS = Map.of(
                "Электроника", new Color(0x4A3DB7), "Одежда", new Color(0xC0392B),
                "Книги", new Color(0x1D9E75), "Спорт", new Color(0xBA7517),
                "Дом и сад", new Color(0x0F6E56), "Игры", new Color(0x7F77DD),
                "Красота", new Color(0xD4537E), "Авто", new Color(0x378ADD),
                "Еда", new Color(0x639922), "Игрушки", new Color(0xD85A30)
        );

        @Override
        public Component getListCellRendererComponent(JList<? extends ProductCard> list, ProductCard card,
                                                      int index, boolean isSelected, boolean cellHasFocus) {
            JPanel cell = new JPanel(new BorderLayout(10, 0));
            cell.setBorder(new EmptyBorder(10, 14, 10, 14));
            cell.setBackground(isSelected ? AppTheme.BG_CARD_SEL : AppTheme.BG_SIDEBAR);

            Color catColor = CAT_COLORS.getOrDefault(card.category(), AppTheme.ACCENT_LAZY);
            JLabel avatar = new JLabel(card.name().substring(0, 1).toUpperCase(), SwingConstants.CENTER);
            avatar.setPreferredSize(new Dimension(44, 44));
            avatar.setFont(new Font("Segoe UI Semibold", Font.PLAIN, 18));
            avatar.setForeground(Color.WHITE);
            avatar.setOpaque(true);
            avatar.setBackground(catColor);

            JPanel avatarWrapper = new RoundPanel(catColor, 22);
            avatarWrapper.setLayout(new BorderLayout());
            avatarWrapper.add(avatar, BorderLayout.CENTER);
            avatarWrapper.setPreferredSize(new Dimension(44, 44));

            JPanel textPanel = new JPanel();
            textPanel.setLayout(new BoxLayout(textPanel, BoxLayout.Y_AXIS));
            textPanel.setOpaque(false);

            JLabel nameLabel = new JLabel(truncate(card.name(), 28));
            nameLabel.setFont(AppTheme.FONT_CARD_NAME);
            nameLabel.setForeground(isSelected ? AppTheme.ACCENT_LAZY : AppTheme.TEXT_PRIMARY);

            JLabel catLabel = new JLabel(card.category());
            catLabel.setFont(AppTheme.FONT_SMALL);
            catLabel.setForeground(AppTheme.TEXT_MUTED);

            JLabel priceLabel = new JLabel(formatPrice(card.price()));
            priceLabel.setFont(AppTheme.FONT_PRICE);
            priceLabel.setForeground(isSelected ? AppTheme.ACCENT_LAZY : AppTheme.TEXT_PRIMARY);

            textPanel.add(nameLabel);
            textPanel.add(Box.createVerticalStrut(2));
            textPanel.add(catLabel);
            textPanel.add(Box.createVerticalStrut(3));
            textPanel.add(priceLabel);

            cell.add(avatarWrapper, BorderLayout.WEST);
            cell.add(textPanel, BorderLayout.CENTER);

            JPanel wrapper = new JPanel(new BorderLayout());
            wrapper.setOpaque(false);
            wrapper.add(cell, BorderLayout.CENTER);
            wrapper.add(new JSeparator() {{ setForeground(AppTheme.BORDER); }}, BorderLayout.SOUTH);
            return wrapper;
        }

        private String truncate(String s, int max) { return s.length() <= max ? s : s.substring(0, max - 1) + "…"; }
        private String formatPrice(double price) { return String.format("%,.0f ₽", price).replace(',', ' '); }
    }

    static class RoundPanel extends JPanel {
        private final Color color; private final int radius;
        RoundPanel(Color color, int radius) { this.color = color; this.radius = radius; setOpaque(false); }
        @Override
        protected void paintComponent(Graphics g) {
            Graphics2D g2 = (Graphics2D) g.create();
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            g2.setColor(color);
            g2.fillRoundRect(0, 0, getWidth(), getHeight(), radius * 2, radius * 2);
            g2.dispose();
            super.paintComponent(g);
        }
    }
}