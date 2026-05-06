package com.gallery.model;

// Полная сущность: содержит "тяжёлые" поля description и tags
public record Item(Long id, String title, String thumbnail, String description, String tags) {
    // Удобный конструктор: берёт базовые данные из Preview + добавляет детали
    public Item(Preview p, String description, String tags) {
        this(p.id(), p.title(), p.thumbnail(), description, tags);
    }
}