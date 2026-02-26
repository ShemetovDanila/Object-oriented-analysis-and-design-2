namespace WITH_pattern
{
    public class Topping
    {
        public string Name { get; set; }
        public int PricePerPortion { get; set; }
        public string Category { get; set; }

        public Topping(string name, int pricePerPortion, string category)
        {
            Name = name;
            PricePerPortion = pricePerPortion;
            Category = category;
        }

        public override string ToString()
        {
            return $"{Name} ({PricePerPortion}₽)";
        }
    }
}