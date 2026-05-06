package com.gallery.db;
import com.gallery.model.*;
import java.sql.*;
import java.util.*;

public class DB {
    private static final String URL = "jdbc:h2:mem:gallery;DB_CLOSE_DELAY=-1";

    // Создание таблицы и 50 записей с "тяжёлым" текстом
    public static void init() throws SQLException {
        try (Connection c = DriverManager.getConnection(URL);
             Statement s = c.createStatement()) {
            s.execute("CREATE TABLE IF NOT EXISTS items (id IDENTITY PRIMARY KEY, title VARCHAR(100), thumbnail VARCHAR(200), description TEXT, tags TEXT)");
            try (PreparedStatement ps = c.prepareStatement("INSERT INTO items (title, thumbnail, description, tags) VALUES (?, ?, ?, ?)")) {
                for (int i = 1; i <= 50; i++) {
                    ps.setString(1, "Фото #" + i);
                    ps.setString(2, "/img/" + i + ".jpg");
                    ps.setString(3, "Описание #".repeat(100) + i); // Имитация больших данных
                    ps.setString(4, "tag1,tag2,tag3".repeat(10));
                    ps.executeUpdate();
                }
            }
        }
    }

    // ❌ EAGER: SELECT * → загружает ВСЕ колонки для ВСЕХ строк сразу
    public static List<Item> findAllEager() throws SQLException {
        List<Item> list = new ArrayList<>();
        try (Connection c = DriverManager.getConnection(URL);
             Statement s = c.createStatement();
             ResultSet rs = s.executeQuery("SELECT * FROM items")) {
            while (rs.next()) {
                list.add(new Item(rs.getLong("id"), rs.getString("title"), rs.getString("thumbnail"),
                                  rs.getString("description"), rs.getString("tags")));
            }
        }
        return list;
    }

    // ✅ LAZY (список): SELECT только 3 лёгких поля
    public static List<Preview> findPreviews() throws SQLException {
        List<Preview> list = new ArrayList<>();
        try (Connection c = DriverManager.getConnection(URL);
             Statement s = c.createStatement();
             ResultSet rs = s.executeQuery("SELECT id, title, thumbnail FROM items")) {
            while (rs.next()) {
                list.add(new Preview(rs.getLong("id"), rs.getString("title"), rs.getString("thumbnail")));
            }
        }
        return list;
    }

    // ✅ LAZY (детали): SELECT * только для ОДНОГО элемента по ID
    public static Item findById(Long id) throws SQLException {
        try (Connection c = DriverManager.getConnection(URL);
             PreparedStatement ps = c.prepareStatement("SELECT * FROM items WHERE id = ?")) {
            ps.setLong(1, id);
            try (ResultSet rs = ps.executeQuery()) {
                if (rs.next()) {
                    return new Item(rs.getLong("id"), rs.getString("title"), rs.getString("thumbnail"),
                                    rs.getString("description"), rs.getString("tags"));
                }
            }
        }
        return null;
    }
}