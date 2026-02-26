using System;
using System.Collections.Generic;

namespace WITH_pattern
{
    public class Pizza
    {
        public string Name { get; set; }
        public int BasePrice { get; set; }
        public bool IsCustomPizza { get; set; }
        public Dough Dough { get; set; }
        public Sauce Sauce { get; set; }
        public Dictionary<Topping, int> Toppings { get; set; }
        public Dictionary<Topping, int> BaseToppings { get; set; }

        public Pizza()
        {
            Toppings = new Dictionary<Topping, int>();
            BaseToppings = new Dictionary<Topping, int>();
        }

        public Pizza(string name, Dough dough, Sauce sauce, int basePrice, bool isCustomPizza)
        {
            Name = name;
            Dough = dough;
            Sauce = sauce;
            BasePrice = basePrice;
            IsCustomPizza = isCustomPizza;
            Toppings = new Dictionary<Topping, int>();
            BaseToppings = new Dictionary<Topping, int>();
        }

        public void AddTopping(Topping topping, int portions, bool isBase)
        {
            if (topping == null) return;

            if (isBase)
            {
                if (BaseToppings.ContainsKey(topping))
                {
                    BaseToppings[topping] += portions;
                }
                else
                {
                    BaseToppings[topping] = portions;
                }

                if (!Toppings.ContainsKey(topping))
                {
                    Toppings[topping] = portions;
                }
            }
            else
            {
                if (Toppings.ContainsKey(topping))
                {
                    Toppings[topping] += portions;
                }
                else
                {
                    Toppings[topping] = portions;
                }
            }
        }

        public int GetTotalPrice()
        {
            if (IsCustomPizza)
            {
                int price = BasePrice;
                if (Dough != null) price += Dough.Price;
                if (Sauce != null) price += Sauce.Price;

                foreach (var kvp in Toppings)
                {
                    price += kvp.Key.PricePerPortion * kvp.Value;
                }

                return price;
            }
            else
            {
                int price = BasePrice;

                foreach (var kvp in Toppings)
                {
                    int basePortions = BaseToppings.ContainsKey(kvp.Key) ? BaseToppings[kvp.Key] : 0;
                    int extraPortions = kvp.Value - basePortions;

                    if (extraPortions > 0)
                    {
                        price += kvp.Key.PricePerPortion * extraPortions;
                    }
                }

                return price;
            }
        }

        public string GetToppingsString()
        {
            if (Toppings.Count == 0)
                return "Без топпингов";

            List<string> list = new List<string>();
            foreach (var kvp in Toppings)
            {
                list.Add($"{kvp.Key.Name} x{kvp.Value}");
            }
            return string.Join(" | ", list);
        }

        public bool IsSameAs(Pizza other)
        {
            if (other == null) return false;
            if (this.Dough?.Name != other.Dough?.Name) return false;
            if (this.Sauce?.Name != other.Sauce?.Name) return false;
            if (this.Toppings.Count != other.Toppings.Count) return false;

            foreach (var kvp in this.Toppings)
            {
                if (!other.Toppings.ContainsKey(kvp.Key)) return false;
                if (other.Toppings[kvp.Key] != kvp.Value) return false;
            }

            return true;
        }

        public override string ToString()
        {
            return $"{Name} [{GetTotalPrice()} руб.]";
        }
    }
}