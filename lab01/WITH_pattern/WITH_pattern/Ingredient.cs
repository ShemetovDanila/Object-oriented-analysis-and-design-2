namespace WITH_pattern
{
    public abstract class Ingredient
    {
        public string name { get; set; }
        public int price { get; set; }
    }

    public class Dough : Ingredient { }
    public class Sauce : Ingredient { }
    public class Topping : Ingredient { }
}