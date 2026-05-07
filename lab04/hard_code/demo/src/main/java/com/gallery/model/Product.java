package com.gallery.model;

/**
 * Полная информация о товаре, включая «тяжёлые» поля:
 * description, specifications, tags.
 * В Lazy-режиме грузится только по требованию (клик на товар).
 * В Eager-режиме грузится сразу вместе со всем каталогом.
 */
public record Product(
        Long id,
        String name,
        double price,
        String category,
        String description,   // «тяжёлое» поле — до 2000 символов
        String specifications,// «тяжёлое» поле — технические хар-ки
        String tags,          // «тяжёлое» поле — список тегов
        int stock,
        double rating
) {
    public static Product fromCard(ProductCard card, String description, String specifications, String tags, int stock, double rating) {
        return new Product(card.id(), card.name(), card.price(), card.category(),
                description, specifications, tags, stock, rating);
    }
}
