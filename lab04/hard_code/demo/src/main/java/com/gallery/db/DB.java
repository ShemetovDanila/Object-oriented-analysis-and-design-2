package com.gallery.db;

import com.gallery.model.Product;
import com.gallery.model.ProductCard;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class DB {
    private static final String DB_URL = "jdbc:h2:mem:shopdb;DB_CLOSE_DELAY=-1";

    private static final String[] CATEGORIES = {
            "Электроника", "Одежда", "Книги", "Спорт", "Дом и сад",
            "Игры", "Красота", "Авто", "Еда", "Игрушки"
    };

    private static final String[] ADJECTIVES = {
            "Профессиональный", "Компактный", "Премиум", "Умный",
            "Беспроводной", "Портативный", "Ультра", "Мини", "Классический", "Новый"
    };

    private static final String[] NOUNS = {
            "Ноутбук", "Наушники", "Камера", "Часы", "Кроссовки",
            "Куртка", "Рюкзак", "Планшет", "Мышь", "Клавиатура",
            "Монитор", "Колонка", "Зарядка", "Кабель", "Чехол",
            "Книга", "Гантели", "Коврик", "Лампа", "Термос"
    };

    private static final String[] BRANDS = {
            "TechPro", "StyleMax", "SmartLine", "UltraTech", "ComfortPlus",
            "NovaBrand", "EcoStyle", "PowerCore", "VisionX", "NextGen"
    };

    public static void initializeDatabase() throws SQLException {
        try (Connection conn = getConnection(); Statement stmt = conn.createStatement()) {
            stmt.execute("""
                    CREATE TABLE IF NOT EXISTS products (
                        id         BIGINT AUTO_INCREMENT PRIMARY KEY,
                        name       VARCHAR(150) NOT NULL,
                        price      DOUBLE       NOT NULL,
                        category   VARCHAR(50)  NOT NULL,
                        stock      INT          NOT NULL,
                        rating     DOUBLE       NOT NULL,
                        description    TEXT,
                        specifications TEXT,
                        tags           TEXT
                    )""");

            // Заполняем, если пусто
            try (ResultSet rs = stmt.executeQuery("SELECT COUNT(*) FROM products")) {
                rs.next();
                if (rs.getInt(1) == 0) {
                    seedDatabase(conn);
                }
            }
        }
    }

    private static void seedDatabase(Connection conn) throws SQLException {
        Random rng = new Random(42);
        String sql = """
                INSERT INTO products
                    (name, price, category, stock, rating, description, specifications, tags)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?)
                """;

        try (PreparedStatement ps = conn.prepareStatement(sql)) {
            for (int i = 1; i <= 60; i++) {
                String adj   = ADJECTIVES[rng.nextInt(ADJECTIVES.length)];
                String noun  = NOUNS[rng.nextInt(NOUNS.length)];
                String brand = BRANDS[rng.nextInt(BRANDS.length)];
                String cat   = CATEGORIES[rng.nextInt(CATEGORIES.length)];

                String name  = brand + " " + adj + " " + noun + " " + i;
                double price = 500 + rng.nextInt(99500);
                int    stock = rng.nextInt(200);
                double rating = 3.0 + rng.nextDouble() * 2.0;

                // «тяжёлые» поля — описания, характеристики, теги
                String description = buildDescription(name, cat, rng);
                String specs = buildSpecifications(cat, rng);
                String tags  = buildTags(cat, noun, brand, rng);

                ps.setString(1, name);
                ps.setDouble(2, price);
                ps.setString(3, cat);
                ps.setInt(4, stock);
                ps.setDouble(5, rating);
                ps.setString(6, description);
                ps.setString(7, specs);
                ps.setString(8, tags);
                ps.addBatch();
            }
            ps.executeBatch();
        }
    }

    // ─────────────────────── LAZY MODE ───────────────────────

    /**
     * [LAZY] Загружает только лёгкие поля (id, name, price, category).
     * Тяжёлые поля (description, specifications, tags) — НЕ грузятся.
     */
    public static List<ProductCard> loadProductCards() throws SQLException {
        List<ProductCard> result = new ArrayList<>();
        String sql = "SELECT id, name, price, category FROM products ORDER BY id";
        try (Connection conn = getConnection();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery(sql)) {
            while (rs.next()) {
                result.add(new ProductCard(
                        rs.getLong("id"),
                        rs.getString("name"),
                        rs.getDouble("price"),
                        rs.getString("category")
                ));
            }
        }
        return result;
    }

    /**
     * [LAZY] Загружает тяжёлые поля только для одного конкретного товара.
     * Вызывается по требованию — когда пользователь кликает на товар.
     */
    public static Product loadProductDetails(long id) throws SQLException {
        String sql = "SELECT * FROM products WHERE id = ?";
        try (Connection conn = getConnection();
             PreparedStatement ps = conn.prepareStatement(sql)) {
            ps.setLong(1, id);
            try (ResultSet rs = ps.executeQuery()) {
                if (rs.next()) {
                    return mapFullProduct(rs);
                }
            }
        }
        return null;
    }

    // ─────────────────────── EAGER MODE ───────────────────────

    /**
     * [EAGER] Загружает ВСЕ поля для ВСЕХ товаров сразу.
     * Тяжёлые поля грузятся даже если пользователь их никогда не увидит.
     */
    public static List<Product> loadAllProducts() throws SQLException {
        List<Product> result = new ArrayList<>();
        String sql = "SELECT * FROM products ORDER BY id";
        try (Connection conn = getConnection();
             Statement stmt = conn.createStatement();
             ResultSet rs = stmt.executeQuery(sql)) {
            while (rs.next()) {
                result.add(mapFullProduct(rs));
            }
        }
        return result;
    }

    // ──────────────────────── HELPERS ────────────────────────

    private static Product mapFullProduct(ResultSet rs) throws SQLException {
        return new Product(
                rs.getLong("id"),
                rs.getString("name"),
                rs.getDouble("price"),
                rs.getString("category"),
                rs.getString("description"),
                rs.getString("specifications"),
                rs.getString("tags"),
                rs.getInt("stock"),
                rs.getDouble("rating")
        );
    }

    private static Connection getConnection() throws SQLException {
        return DriverManager.getConnection(DB_URL);
    }

    private static String buildDescription(String name, String cat, Random rng) {
        String[] intros = {
                "Представляем вашему вниманию великолепный продукт — ",
                "Откройте для себя ",
                "Познакомьтесь с выдающимся товаром — ",
                "Мы рады предложить вам "
        };
        String intro = intros[rng.nextInt(intros.length)] + name + ". ";
        String body = "Это изделие из категории «" + cat + "» создано с использованием " +
                "современных технологий и высококачественных материалов. " +
                "Благодаря тщательной проработке каждой детали, данный товар обеспечивает " +
                "максимальный комфорт и долговечность при повседневном использовании. " +
                "Разработанный с учётом потребностей современного покупателя, он станет " +
                "незаменимым помощником в вашей жизни. Продукт прошёл строгий контроль " +
                "качества и соответствует всем современным стандартам безопасности. " +
                "Сотни довольных покупателей уже оценили его преимущества. " +
                "Гарантия производителя подтверждает высокое качество и надёжность товара. ".repeat(3);
        return intro + body;
    }

    private static String buildSpecifications(String cat, Random rng) {
        return switch (cat) {
            case "Электроника" -> String.format(
                    "Процессор: %d ГГц | ОЗУ: %d ГБ | Накопитель: %d ГБ SSD | " +
                    "Экран: %d\" %dp | Батарея: %d мАч | Вес: %.1f кг | Гарантия: %d мес.",
                    2 + rng.nextInt(4), 8 * (1 + rng.nextInt(4)),
                    256 * (1 + rng.nextInt(8)), 13 + rng.nextInt(8),
                    720 + 360 * rng.nextInt(4), 3000 + rng.nextInt(9000),
                    0.5 + rng.nextDouble() * 2, 12 + rng.nextInt(25));
            case "Одежда" -> String.format(
                    "Материал: %s | Размеры: XS–XXL | Цвета: %d варианта | " +
                    "Уход: машинная стирка %d°C | Страна: %s",
                    new String[]{"100% хлопок", "полиэстер", "шерсть 80%%"}[rng.nextInt(3)],
                    2 + rng.nextInt(6), 30 + 10 * rng.nextInt(4),
                    new String[]{"Россия", "Китай", "Португалия"}[rng.nextInt(3)]);
            default -> String.format(
                    "Материал: высококачественный пластик/металл | " +
                    "Размеры: %d×%d×%d мм | Вес: %d г | Цвет: %d варианта | Гарантия: %d мес.",
                    50 + rng.nextInt(200), 50 + rng.nextInt(150), 10 + rng.nextInt(50),
                    100 + rng.nextInt(900), 1 + rng.nextInt(5), 6 + rng.nextInt(19));
        };
    }

    private static String buildTags(String cat, String noun, String brand, Random rng) {
        List<String> tags = new ArrayList<>();
        tags.add(cat.toLowerCase());
        tags.add(noun.toLowerCase());
        tags.add(brand.toLowerCase());
        tags.add(rng.nextBoolean() ? "новинка" : "хит продаж");
        tags.add(rng.nextBoolean() ? "скидка" : "рекомендуем");
        if (rng.nextBoolean()) tags.add("эко");
        if (rng.nextBoolean()) tags.add("премиум");
        return String.join(", ", tags);
    }
}
