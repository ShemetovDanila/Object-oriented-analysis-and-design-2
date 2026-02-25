using System;

namespace WinFormsApp1
{
    public class Pizza
    {
        public string Name { get; set; }
        public int Price { get; set; }
        public string Dough { get; set; }
        public string Sauce { get; set; }

        public Pizza(string name, int price, string dough, string sauce)
        {
            Name = name;
            Price = price;
            Dough = dough;
            Sauce = sauce;
        }

        public int GetPrice()
        {
            return Price;
        }

        public override string ToString()
        {
            return $"{Name} ({Dough}, {Sauce})";
        }
    }
}