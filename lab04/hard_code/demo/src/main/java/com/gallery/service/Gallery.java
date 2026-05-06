package com.gallery.service;
import com.gallery.db.DB;
import com.gallery.model.*;
import java.util.*;

public class Gallery {
    // Замер времени и вызов EAGER-загрузки
    public static List<Item> loadAllEager() {
        long start = System.currentTimeMillis();
        try {
            List<Item> items = DB.findAllEager();
            System.out.println("⏱️ EAGER: " + items.size() + " эл. за " + (System.currentTimeMillis() - start) + " мс");
            return items;
        } catch (Exception e) { throw new RuntimeException(e); }
    }

    // Замер времени и вызов LAZY-загрузки списка
    public static List<Preview> loadPreviews() {
        long start = System.currentTimeMillis();
        try {
            List<Preview> list = DB.findPreviews();
            System.out.println("⏱️ LAZY (список): " + list.size() + " эл. за " + (System.currentTimeMillis() - start) + " мс");
            return list;
        } catch (Exception e) { throw new RuntimeException(e); }
    }

    // Замер времени и вызов LAZY-загрузки деталей
    public static Item loadDetails(Long id) {
        long start = System.currentTimeMillis();
        try {
            Item item = DB.findById(id);
            System.out.println("⏱️ LAZY (детали #" + id + "): " + (System.currentTimeMillis() - start) + " мс");
            return item;
        } catch (Exception e) { throw new RuntimeException(e); }
    }
}