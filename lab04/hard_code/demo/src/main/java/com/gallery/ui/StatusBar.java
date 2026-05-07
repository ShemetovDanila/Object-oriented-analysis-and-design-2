package com.gallery.ui;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;

/**
 * Нижняя строка состояния — режим загрузки, статистика, статус.
 */
public class StatusBar extends JPanel {

    private final JLabel modeLabel   = new JLabel();
    private final JLabel statusLabel = new JLabel("Готово");
    private final JLabel statsLabel  = new JLabel();
    private final JProgressBar progress = new JProgressBar();

    public StatusBar() {
        setLayout(new BorderLayout(12, 0));
        setBackground(AppTheme.BG_PRIMARY);
        setBorder(new CompoundBorder(
                new MatteBorder(1, 0, 0, 0, AppTheme.BORDER),
                new EmptyBorder(6, 16, 6, 16)));

        // Левая часть — режим
        modeLabel.setFont(AppTheme.FONT_BADGE);
        modeLabel.setBorder(new EmptyBorder(2, 8, 2, 8));
        modeLabel.setOpaque(true);

        JPanel leftPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 8, 0));
        leftPanel.setOpaque(false);
        leftPanel.add(modeLabel);

        // Центр — статус
        statusLabel.setFont(AppTheme.FONT_SMALL);
        statusLabel.setForeground(AppTheme.TEXT_SECONDARY);
        statusLabel.setHorizontalAlignment(SwingConstants.CENTER);

        // Правая часть — статистика
        statsLabel.setFont(AppTheme.FONT_SMALL);
        statsLabel.setForeground(AppTheme.TEXT_MUTED);

        progress.setIndeterminate(true);
        progress.setPreferredSize(new Dimension(80, 12));
        progress.setVisible(false);
        progress.setStringPainted(false);

        JPanel rightPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 8, 0));
        rightPanel.setOpaque(false);
        rightPanel.add(progress);
        rightPanel.add(statsLabel);

        add(leftPanel, BorderLayout.WEST);
        add(statusLabel, BorderLayout.CENTER);
        add(rightPanel, BorderLayout.EAST);

        setLazyMode(); // По умолчанию
    }

    public void setLazyMode() {
        modeLabel.setText("LAZY — загрузка по требованию");
        modeLabel.setBackground(new Color(0xE8E4F8));
        modeLabel.setForeground(AppTheme.ACCENT_LAZY);
    }

    public void setEagerMode() {
        modeLabel.setText("EAGER — загрузка всего сразу");
        modeLabel.setBackground(new Color(0xFAECEB));
        modeLabel.setForeground(AppTheme.ACCENT_EAGER);
    }

    public void setStatus(String text) {
        SwingUtilities.invokeLater(() -> statusLabel.setText(text));
    }

    public void setStats(String text) {
        SwingUtilities.invokeLater(() -> statsLabel.setText(text));
    }

    public void setLoading(boolean loading) {
        SwingUtilities.invokeLater(() -> progress.setVisible(loading));
    }
}
