package com.gallery.model;

/**
 * Лёгкая карточка товара — только то, что нужно для отображения в списке.
 * Грузится всегда (и в Lazy, и в Eager режиме).
 */
public record ProductCard(Long id, String name, double price, String category) {
    @Override
    public String toString() {
        return name;
    }
}
