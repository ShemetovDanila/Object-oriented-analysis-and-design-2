package com.gallery.service;

import com.gallery.db.DB;
import com.gallery.model.Product;
import com.gallery.model.ProductCard;

import java.util.List;

/**
 * Сервисный слой — содержит бизнес-логику и замеряет время запросов.
 */
public class CatalogService {

    public record LoadResult<T>(T data, long elapsedMs, int bytesEstimate) {}

    // ──────────────────────── LAZY MODE ────────────────────────

    /**
     * Загружает только список карточек (лёгкие поля).
     * Тяжёлые поля не трогаются.
     */
    public static LoadResult<List<ProductCard>> loadCards() {
        long t0 = System.currentTimeMillis();
        try {
            List<ProductCard> cards = DB.loadProductCards();
            long elapsed = System.currentTimeMillis() - t0;
            // Примерный вес: id(8) + name(~40) + price(8) + category(~12) = ~68 байт
            int bytes = cards.size() * 68;
            System.out.printf("[LAZY]  Список загружен: %d карточек за %d мс (~%d КБ)%n",
                    cards.size(), elapsed, bytes / 1024);
            return new LoadResult<>(cards, elapsed, bytes);
        } catch (Exception e) {
            throw new RuntimeException("Ошибка загрузки списка", e);
        }
    }

    /**
     * Загружает полные данные одного товара — по требованию.
     */
    public static LoadResult<Product> loadDetails(long id) {
        long t0 = System.currentTimeMillis();
        try {
            Product p = DB.loadProductDetails(id);
            long elapsed = System.currentTimeMillis() - t0;
            int bytes = p == null ? 0 :
                    (p.description().length() + p.specifications().length() + p.tags().length()) * 2;
            System.out.printf("[LAZY]  Детали товара #%d загружены за %d мс (~%d байт)%n",
                    id, elapsed, bytes);
            return new LoadResult<>(p, elapsed, bytes);
        } catch (Exception e) {
            throw new RuntimeException("Ошибка загрузки деталей товара #" + id, e);
        }
    }

    // ──────────────────────── EAGER MODE ────────────────────────

    /**
     * Загружает ВСЕ товары со ВСЕМИ полями сразу.
     * Тяжёлые поля грузятся даже если пользователь их не откроет.
     */
    public static LoadResult<List<Product>> loadAll() {
        long t0 = System.currentTimeMillis();
        try {
            List<Product> all = DB.loadAllProducts();
            long elapsed = System.currentTimeMillis() - t0;
            // Примерный вес с описаниями (~1500 симв) + хар-ки (~200) + теги (~50)
            int bytes = all.size() * (68 + 1500 + 200 + 50) * 2;
            System.out.printf("[EAGER] Все товары загружены: %d шт. за %d мс (~%d КБ)%n",
                    all.size(), elapsed, bytes / 1024);
            return new LoadResult<>(all, elapsed, bytes);
        } catch (Exception e) {
            throw new RuntimeException("Ошибка загрузки всех товаров", e);
        }
    }
}
