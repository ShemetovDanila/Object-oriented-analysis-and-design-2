using System.Collections.Generic;
using System.Linq;

namespace WITH_pattern
{
    public class Order
    {
        public List<OrderItem> Items { get; set; } = new List<OrderItem>();

        public void AddPizza(Pizza pizza)
        {
            var existing = Items.FirstOrDefault(i =>
                i.Pizza.name == pizza.name &&
                i.Pizza.Dough == pizza.Dough &&
                i.Pizza.Sauce == pizza.Sauce &&
                i.Pizza.GetToppingsString() == pizza.GetToppingsString());

            if (existing != null)
            {
                existing.Quantity++;
            }
            else
            {
                Items.Add(new OrderItem(pizza));
            }
        }

        public void RemoveItem(OrderItem item)
        {
            Items.Remove(item);
        }

        public int GetTotalSum()
        {
            return Items.Sum(i => i.GetTotalPrice());
        }

        public int GetTotalPizzasCount()
        {
            return Items.Sum(i => i.Quantity);
        }
    }
}