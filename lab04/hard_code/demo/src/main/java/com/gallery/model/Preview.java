package com.gallery.model;

// Лёгкий DTO: только поля, нужные для быстрого отображения списка
public record Preview(Long id, String title, String thumbnail) {}