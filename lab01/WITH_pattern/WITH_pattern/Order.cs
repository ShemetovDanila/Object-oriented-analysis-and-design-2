using System;
using System.Collections.Generic;
using System.Linq;

namespace WITH_pattern
{
    public class Order
    {
        public int orderId { get; set; }
        public string orderDate { get; set; }
        public string status { get; set; }
        public List<Pizza> Pizzas { get; set; } = new List<Pizza>();

        public void AddPizza(Pizza pizza) => Pizzas.Add(pizza);

        public int GetTotalOrderPrice() => Pizzas.Sum(p => p.CalculateTotal());
    }
}