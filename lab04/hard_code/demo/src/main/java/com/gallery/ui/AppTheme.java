package com.gallery.ui;

import java.awt.*;

/**
 * Цветовая палитра и шрифты всего приложения.
 */
public final class AppTheme {

    // ── Основные цвета ──────────────────────────────────────────
    public static final Color BG_PRIMARY     = new Color(0xF8F7F4);   // фон окна
    public static final Color BG_SIDEBAR     = new Color(0xFFFFFF);   // боковая панель
    public static final Color BG_DETAIL      = new Color(0xFFFFFF);   // панель деталей
    public static final Color BG_CARD_HOVER  = new Color(0xF0EEE8);   // карточка при наведении
    public static final Color BG_CARD_SEL    = new Color(0xE8E4F8);   // выбранная карточка
    public static final Color BG_HEADER      = new Color(0x1E1B2E);   // шапка

    // ── Акцентные ───────────────────────────────────────────────
    public static final Color ACCENT_LAZY    = new Color(0x4A3DB7);   // фиолетовый — паттерн
    public static final Color ACCENT_EAGER   = new Color(0xC0392B);   // красный — eager
    public static final Color ACCENT_SUCCESS = new Color(0x1D9E75);   // зелёный — ОК
    public static final Color ACCENT_AMBER   = new Color(0xBA7517);   // янтарный — предупреждение

    // ── Текст ───────────────────────────────────────────────────
    public static final Color TEXT_PRIMARY   = new Color(0x1E1B2E);
    public static final Color TEXT_SECONDARY = new Color(0x6B6880);
    public static final Color TEXT_MUTED     = new Color(0x9E9BAE);
    public static final Color TEXT_ON_DARK   = new Color(0xF5F4FC);
    public static final Color TEXT_ON_ACCENT = Color.WHITE;

    // ── Границы ─────────────────────────────────────────────────
    public static final Color BORDER         = new Color(0xE0DEDB);
    public static final Color BORDER_FOCUS   = new Color(0x7F77DD);

    // ── Шрифты ──────────────────────────────────────────────────
    public static final Font FONT_TITLE    = new Font("Segoe UI Semibold", Font.PLAIN, 20);
    public static final Font FONT_SUBTITLE = new Font("Segoe UI", Font.PLAIN, 13);
    public static final Font FONT_BODY     = new Font("Segoe UI", Font.PLAIN, 14);
    public static final Font FONT_SMALL    = new Font("Segoe UI", Font.PLAIN, 12);
    public static final Font FONT_MONO     = new Font("Consolas", Font.PLAIN, 12);
    public static final Font FONT_BADGE    = new Font("Segoe UI", Font.BOLD, 11);
    public static final Font FONT_CARD_NAME= new Font("Segoe UI Semibold", Font.PLAIN, 14);
    public static final Font FONT_PRICE    = new Font("Segoe UI Semibold", Font.PLAIN, 15);
    public static final Font FONT_HEADER   = new Font("Segoe UI Semibold", Font.PLAIN, 15);

    private AppTheme() {}
}
