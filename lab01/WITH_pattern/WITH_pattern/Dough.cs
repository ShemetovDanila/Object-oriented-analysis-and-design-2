namespace WITH_pattern
{
    public class Dough
    {
        public string Name { get; set; }
        public int Price { get; set; }

        public Dough(string name, int price)
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