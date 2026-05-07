package com.gallery.ui;

import com.gallery.model.Product;
import com.gallery.model.ProductCard;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;

public class ProductDetailPanel extends JPanel {
    private final JLabel nameLabel = new JLabel();
    private final JLabel categoryLabel = new JLabel();
    private final JLabel priceLabel = new JLabel();
    private final JLabel ratingLabel = new JLabel();
    private final JLabel stockLabel = new JLabel();
    private final JTextArea descArea = new JTextArea();
    private final JTextArea specsArea = new JTextArea();
    private final JLabel tagsLabel = new JLabel();

    private final JPanel loadingPanel;
    private final JPanel contentPanel;
    private final JPanel emptyPanel;
    private final JPanel lazyHint;

    private final JLabel loadTimeLabel = new JLabel();
    private final JLabel loadSizeLabel = new JLabel();

    public ProductDetailPanel() {
        setLayout(new CardLayout());
        setBackground(AppTheme.BG_DETAIL);

        emptyPanel = buildEmptyPanel(); add(emptyPanel, "EMPTY");
        loadingPanel = buildLoadingPanel(); add(loadingPanel, "LOADING");
        contentPanel = buildContentPanel(); add(contentPanel, "CONTENT");
        lazyHint = buildLazyHintPanel(); add(lazyHint, "LAZY_HINT");
        showEmpty();
    }

    public void showEmpty() { ((CardLayout) getLayout()).show(this, "EMPTY"); }
    public void showLoading() { ((CardLayout) getLayout()).show(this, "LOADING"); }

    public void showLazyHint(ProductCard card) {
        JLabel hint = (JLabel) ((JPanel) lazyHint.getComponent(1)).getComponent(1);
        hint.setText("<html>Выбран: <b>" + card.name() + "</b><br>Нажмите кнопку ниже, чтобы загрузить описание и характеристики</html>");
        ((CardLayout) getLayout()).show(this, "LAZY_HINT");
    }

    public void showProduct(Product p, long loadMs, int loadBytes) {
        nameLabel.setText(p.name());
        categoryLabel.setText(p.category());
        priceLabel.setText(formatPrice(p.price()));
        ratingLabel.setText(String.format("Рейтинг: %.1f / 5.0", p.rating()));
        stockLabel.setText(p.stock() > 0 ? "В наличии: " + p.stock() + " шт." : "Нет в наличии");
        stockLabel.setForeground(p.stock() > 0 ? AppTheme.ACCENT_SUCCESS : AppTheme.ACCENT_EAGER);
        descArea.setText(p.description());
        specsArea.setText(p.specifications());
        tagsLabel.setText(" " + buildTagsHtml(p.tags()) + " ");
        loadTimeLabel.setText(loadMs + " мс");
        loadSizeLabel.setText(loadBytes > 1024 ? String.format("%.1f КБ", loadBytes / 1024.0) : loadBytes + " Б");
        ((CardLayout) getLayout()).show(this, "CONTENT");
    }

    public void showProductEager(Product p) {
        showProduct(p, 0, 0);
        loadTimeLabel.setText("—"); loadSizeLabel.setText("—");
    }

    private JPanel buildEmptyPanel() {
        JPanel p = new JPanel(new GridBagLayout());
        p.setBackground(AppTheme.BG_DETAIL);
        JLabel msg = new JLabel("📦 Выберите товар из списка слева");
        msg.setFont(AppTheme.FONT_BODY); msg.setForeground(AppTheme.TEXT_MUTED);
        p.add(msg); return p;
    }

    private JPanel buildLoadingPanel() {
        JPanel p = new JPanel(new GridBagLayout());
        p.setBackground(AppTheme.BG_DETAIL);
        JLabel msg = new JLabel("Загрузка данных...");
        msg.setFont(AppTheme.FONT_BODY); msg.setForeground(AppTheme.TEXT_SECONDARY);
        p.add(msg); return p;
    }

    private JPanel buildLazyHintPanel() {
        JPanel outer = new JPanel(new GridBagLayout()); outer.setBackground(AppTheme.BG_DETAIL);
        JPanel inner = new JPanel();
        inner.setLayout(new BoxLayout(inner, BoxLayout.Y_AXIS));
        inner.setBackground(AppTheme.BG_DETAIL);
        inner.setBorder(new EmptyBorder(32, 40, 32, 40));
        JLabel icon = new JLabel("", SwingConstants.CENTER);
        icon.setFont(new Font("Segoe UI", Font.PLAIN, 48)); icon.setAlignmentX(CENTER_ALIGNMENT);
        JLabel hint = new JLabel("", SwingConstants.CENTER);
        hint.setFont(AppTheme.FONT_BODY); hint.setForeground(AppTheme.TEXT_SECONDARY); hint.setAlignmentX(CENTER_ALIGNMENT);
        inner.add(icon); inner.add(Box.createVerticalStrut(16)); inner.add(hint);
        outer.add(inner); return outer;
    }

    private JPanel buildContentPanel() {
        JPanel root = new JPanel(new BorderLayout());
        root.setBackground(AppTheme.BG_DETAIL);

        JPanel header = new JPanel(new BorderLayout(12, 4));
        header.setBackground(AppTheme.BG_DETAIL); header.setBorder(new EmptyBorder(24, 24, 16, 24));
        nameLabel.setFont(AppTheme.FONT_TITLE); nameLabel.setForeground(AppTheme.TEXT_PRIMARY);
        categoryLabel.setFont(AppTheme.FONT_SMALL); categoryLabel.setForeground(AppTheme.TEXT_MUTED);
        priceLabel.setFont(new Font("Segoe UI Semibold", Font.PLAIN, 24)); priceLabel.setForeground(AppTheme.ACCENT_LAZY);

        JPanel nameBlock = new JPanel();
        nameBlock.setLayout(new BoxLayout(nameBlock, BoxLayout.Y_AXIS)); nameBlock.setOpaque(false);
        nameBlock.add(categoryLabel); nameBlock.add(Box.createVerticalStrut(4)); nameBlock.add(nameLabel);

        JPanel headerRight = new JPanel();
        headerRight.setLayout(new BoxLayout(headerRight, BoxLayout.Y_AXIS)); headerRight.setOpaque(false);
        headerRight.add(priceLabel);

        header.add(nameBlock, BorderLayout.CENTER); header.add(headerRight, BorderLayout.EAST);

        JPanel metaRow = new JPanel(new FlowLayout(FlowLayout.LEFT, 8, 0));
        metaRow.setOpaque(false); metaRow.setBorder(new EmptyBorder(0, 24, 12, 24));
        ratingLabel.setFont(AppTheme.FONT_BODY); ratingLabel.setForeground(AppTheme.ACCENT_AMBER);
        stockLabel.setFont(AppTheme.FONT_SMALL);
        metaRow.add(ratingLabel);
        metaRow.add(new JSeparator(SwingConstants.VERTICAL) {{ setPreferredSize(new Dimension(1, 16)); }});
        metaRow.add(stockLabel);

        JSeparator sep = new JSeparator(); sep.setForeground(AppTheme.BORDER);
        JPanel headerBlock = new JPanel(new BorderLayout());
        headerBlock.setOpaque(false); headerBlock.setBorder(new MatteBorder(0, 0, 1, 0, AppTheme.BORDER));
        headerBlock.add(header, BorderLayout.CENTER); headerBlock.add(metaRow, BorderLayout.SOUTH);
        root.add(headerBlock, BorderLayout.NORTH);

        JPanel content = new JPanel();
        content.setLayout(new BoxLayout(content, BoxLayout.Y_AXIS));
        content.setBackground(AppTheme.BG_DETAIL); content.setBorder(new EmptyBorder(0, 24, 24, 24));

        content.add(sectionHeader("Описание"));
        descArea.setFont(AppTheme.FONT_BODY); descArea.setForeground(AppTheme.TEXT_SECONDARY);
        descArea.setBackground(AppTheme.BG_PRIMARY); descArea.setEditable(false);
        descArea.setLineWrap(true); descArea.setWrapStyleWord(true);
        descArea.setBorder(new CompoundBorder(new LineBorder(AppTheme.BORDER, 1, true), new EmptyBorder(10, 12, 10, 12)));
        descArea.setRows(5); content.add(descArea); content.add(Box.createVerticalStrut(16));

        content.add(sectionHeader("Характеристики"));
        specsArea.setFont(AppTheme.FONT_MONO); specsArea.setForeground(AppTheme.TEXT_SECONDARY);
        specsArea.setBackground(AppTheme.BG_PRIMARY); specsArea.setEditable(false);
        specsArea.setLineWrap(true); specsArea.setWrapStyleWord(true);
        specsArea.setBorder(new CompoundBorder(new LineBorder(AppTheme.BORDER, 1, true), new EmptyBorder(10, 12, 10, 12)));
        specsArea.setRows(3); content.add(specsArea); content.add(Box.createVerticalStrut(16));

        content.add(sectionHeader("Теги"));
        tagsLabel.setFont(AppTheme.FONT_SMALL); tagsLabel.setForeground(AppTheme.TEXT_SECONDARY);
        tagsLabel.setBorder(new EmptyBorder(4, 0, 4, 0)); content.add(tagsLabel);
        content.add(Box.createVerticalStrut(20));
        content.add(buildLoadStatsPanel());

        // Скролл с единым стилем
        JScrollPane scroll = new JScrollPane(content);
        scroll.setBorder(null);
        scroll.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
        
        // Вызываем утилиту для стилизации
        UIUtils.applyScrollBarStyle(scroll, new Color(0xC8C5D8), AppTheme.BG_DETAIL);
        
        root.add(scroll, BorderLayout.CENTER);
        return root;
    }

    private JPanel buildLoadStatsPanel() {
        JPanel p = new JPanel(new FlowLayout(FlowLayout.LEFT, 12, 4));
        p.setBackground(AppTheme.BG_PRIMARY);
        p.setBorder(new CompoundBorder(new LineBorder(AppTheme.BORDER, 1, true), new EmptyBorder(6, 12, 6, 12)));
        p.setMaximumSize(new Dimension(Integer.MAX_VALUE, 40));
        JLabel title = new JLabel("Загрузка деталей:"); title.setFont(AppTheme.FONT_SMALL); title.setForeground(AppTheme.TEXT_MUTED);
        loadTimeLabel.setFont(AppTheme.FONT_BADGE); loadTimeLabel.setForeground(AppTheme.ACCENT_LAZY);
        JLabel timeLbl = new JLabel("за"); timeLbl.setFont(AppTheme.FONT_SMALL); timeLbl.setForeground(AppTheme.TEXT_MUTED);
        JLabel sizeLbl = new JLabel(" • объём:"); sizeLbl.setFont(AppTheme.FONT_SMALL); sizeLbl.setForeground(AppTheme.TEXT_MUTED);
        loadSizeLabel.setFont(AppTheme.FONT_BADGE); loadSizeLabel.setForeground(AppTheme.ACCENT_LAZY);
        p.add(title); p.add(loadTimeLabel); p.add(timeLbl); p.add(sizeLbl); p.add(loadSizeLabel);
        return p;
    }

    private Component sectionHeader(String text) {
        JLabel lbl = new JLabel(text);
        lbl.setFont(new Font("Segoe UI Semibold", Font.PLAIN, 13));
        lbl.setForeground(AppTheme.TEXT_MUTED);
        lbl.setBorder(new EmptyBorder(12, 0, 6, 0));
        lbl.setAlignmentX(LEFT_ALIGNMENT);
        return lbl;
    }

    private String formatPrice(double price) { return String.format("%,.0f ₽", price).replace(',', ' '); }
    private String buildTagsHtml(String tags) {
        if (tags == null || tags.isEmpty()) return "нет тегов";
        StringBuilder sb = new StringBuilder();
        for (String t : tags.split(",")) sb.append(" ").append(t.trim()).append("  ");
        return sb.toString();
    }
}