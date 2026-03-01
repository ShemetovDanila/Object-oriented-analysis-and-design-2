using System.Collections.Generic;
using System.Linq;

namespace WITH_pattern
{
    public class Pizza
    {
        public string name { get; set; }
        public int price { get; set; }
        public bool custom { get; set; }
        public Dough Dough { get; set; }
        public Sauce Sauce { get; set; }
        public List<Topping> Toppings { get; set; } = new List<Topping>();

        public int CalculateTotal()
        {
            int total = price;
            if (Dough != null) total += Dough.price;
            if (Sauce != null) total += Sauce.price;
            total += Toppings.Sum(t => t.price);
            return total;
        }

        public string GetDescription()
        {
            var summary = new List<string>();
            if (Dough != null) summary.Add(Dough.name);
            if (Sauce != null) summary.Add(Sauce.name);
            if (Toppings.Count > 0)
                summary.Add(string.Join(", ", Toppings.GroupBy(t => t.name).Select(g => $"{g.Key} x{g.Count()}")));
            else summary.Add("Без топпингов");

            return string.Join(" | ", summary);
        }
    }
}