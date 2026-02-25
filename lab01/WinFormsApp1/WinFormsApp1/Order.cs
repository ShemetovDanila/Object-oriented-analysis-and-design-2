using System;
using System.Collections.Generic;

namespace WinFormsApp1
{
    public class Order
    {
        public List<OrderItem> Items { get; set; }

        public Order()
        {
            Items = new List<OrderItem>();
        }

        public void AddPizza(Pizza pizza)
        {
            foreach (OrderItem item in Items)
            {
                if (item.Pizza.IsSameAs(pizza))
                {
                    item.Quantity++;
                    item.UpdateQuantityDisplay();
                    return;
                }
            }

            OrderItem newItem = new OrderItem(pizza, 1);
            Items.Add(newItem);
        }

        public void RemoveItem(OrderItem item)
        {
            Items.Remove(item);
        }

        public int GetTotalSum()
        {
            int sum = 0;
            foreach (OrderItem item in Items)
            {
                sum += item.GetTotalPrice();
            }
            return sum;
        }

        public int GetTotalPizzasCount()
        {
            int count = 0;
            foreach (OrderItem item in Items)
            {
                count += item.Quantity;
            }
            return count;
        }
    }
}