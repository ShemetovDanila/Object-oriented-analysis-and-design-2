namespace WITH_pattern
{
    public class Sauce
    {
        public string Name { get; set; }
        public int Price { get; set; }

        public Sauce(string name, int price)
        {
            Name = name;
            Price = price;
        }

        public override string ToString()
        {
            return $"{Name} ({Price}₽)";
        }
    }
}