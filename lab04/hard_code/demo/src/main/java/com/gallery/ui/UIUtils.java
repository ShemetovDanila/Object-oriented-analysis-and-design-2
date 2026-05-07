package com.gallery.ui;

import javax.swing.*;
import javax.swing.plaf.basic.BasicScrollBarUI;
import java.awt.*;

/**
 * Утилита для единой стилизации скроллбаров во всём приложении.
 */
public class UIUtils {

    public static void applyScrollBarStyle(JScrollPane scrollPane, Color thumb, Color track) {
        if (scrollPane == null) return;
        styleBar(scrollPane.getVerticalScrollBar(), thumb, track);
        styleBar(scrollPane.getHorizontalScrollBar(), thumb, track);
    }

    private static void styleBar(JScrollBar bar, Color thumb, Color track) {
        if (bar == null) return;

        bar.setUI(new BasicScrollBarUI() {
            @Override
            protected void configureScrollBarColors() {
                this.thumbColor = thumb;
                this.trackColor = track;
            }

            @Override
            protected void paintThumb(Graphics g, JComponent c) {
                if (thumbRect != null && !thumbRect.isEmpty()) {
                    Graphics2D g2 = (Graphics2D) g.create();
                    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                    g2.setColor(thumbColor);
                    int radius = Math.min(thumbRect.width, thumbRect.height) / 2;
                    g2.fillRoundRect(thumbRect.x, thumbRect.y, thumbRect.width, thumbRect.height, radius, radius);
                    g2.dispose();
                }
            }

            @Override
            protected void paintTrack(Graphics g, JComponent c) {
                if (trackRect != null && !trackRect.isEmpty()) {
                    Graphics2D g2 = (Graphics2D) g.create();
                    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                    g2.setColor(trackColor);
                    g2.fillRect(trackRect.x, trackRect.y, trackRect.width, trackRect.height);
                    g2.dispose();
                }
            }

            @Override
            protected JButton createDecreaseButton(int orientation) {
                return createZeroButton();
            }

            @Override
            protected JButton createIncreaseButton(int orientation) {
                return createZeroButton();
            }

            private JButton createZeroButton() {
                JButton b = new JButton();
                b.setPreferredSize(new Dimension(0, 0));
                b.setMinimumSize(new Dimension(0, 0));
                b.setMaximumSize(new Dimension(0, 0));
                return b;
            }
        });

        bar.setUnitIncrement(16);
        bar.setPreferredSize(bar.getOrientation() == JScrollBar.VERTICAL
                ? new Dimension(10, 0)
                : new Dimension(0, 10));
    }
}