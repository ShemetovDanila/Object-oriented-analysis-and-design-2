package com.gallery.ui;
import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;

public class ModeHeaderPanel extends JPanel {
    private final JLabel titleLabel = new JLabel();
    private final JLabel descLabel  = new JLabel();

    public ModeHeaderPanel() {
        // Убрал иконку, оставил только текст по центру или слева
        setLayout(new BorderLayout(14, 0));
        
        JPanel textBlock = new JPanel();
        textBlock.setLayout(new BoxLayout(textBlock, BoxLayout.Y_AXIS));
        textBlock.setOpaque(false);

        titleLabel.setFont(new Font("Segoe UI Semibold", Font.PLAIN, 15));
        descLabel.setFont(AppTheme.FONT_SMALL);

        textBlock.add(titleLabel);
        textBlock.add(Box.createVerticalStrut(2));
        textBlock.add(descLabel);

        add(textBlock, BorderLayout.CENTER);

        showLazy();
    }

    public void showLazy() {
        setBackground(new Color(0xEEEDFE));
        setBorder(new CompoundBorder(
                new MatteBorder(0, 0, 1, 0, new Color(0xCECBF6)),
                new EmptyBorder(12, 20, 12, 20)));
        
        titleLabel.setText("Режим: Загрузка по требованию (Lazy Load)");
        titleLabel.setForeground(new Color(0x3C3489));
        descLabel.setText("Базовые поля (название, цена, категория) загружаются для всего каталога. " +
                "Описание, характеристики и теги — только при выборе конкретного товара.");
        descLabel.setForeground(new Color(0x534AB7));
    }

    public void showEager() {
        setBackground(new Color(0xFCEBEB));
        setBorder(new CompoundBorder(
                new MatteBorder(0, 0, 1, 0, new Color(0xF7C1C1)),
                new EmptyBorder(12, 20, 12, 20)));
        
        titleLabel.setText("Режим: Без паттерна (Eager Load)");
        titleLabel.setForeground(new Color(0x791F1F));
        descLabel.setText("Все поля всех товаров загружаются сразу при открытии каталога. " +
                "Описания и характеристики грузятся даже если вы их не откроете.");
        descLabel.setForeground(new Color(0xA32D2D));
    }
}