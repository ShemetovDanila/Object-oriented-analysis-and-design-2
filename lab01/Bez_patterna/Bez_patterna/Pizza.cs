using System;
using System.Collections.Generic;

namespace Bez_patterna
{
    public class Pizza
    {
        public string Name { get; set; }
        public int BasePrice { get; set; }

        public string DoughType { get; set; }
        public string SauceType { get; set; }

        public int MozzarellaPortions { get; set; }
        public int CheddarPortions { get; set; }
        public int PepperoniPortions { get; set; }
        public int JalapenoPortions { get; set; }

        public int DefaultMozzarella { get; set; }
        public int DefaultCheddar { get; set; }
        public int DefaultPepperoni { get; set; }
        public int DefaultJalapeno { get; set; }

        public bool HasChicken { get; set; }
        public bool HasMushrooms { get; set; }
        public bool HasTomatoes { get; set; }
        public bool HasBasil { get; set; }
        public bool HasHam { get; set; }
        public bool HasPineapple { get; set; }

        public Pizza(
            string name,
            int basePrice,
            string doughType,
            string sauceType,
            int mozzarellaPortions,
            int cheddarPortions,
            int pepperoniPortions,
            int jalapenoPortions,
            int defaultMozzarella,
            int defaultCheddar,
            int defaultPepperoni,
            int defaultJalapeno,
            bool hasChicken,
            bool hasMushrooms,
            bool hasTomatoes,
            bool hasBasil,
            bool hasHam,
            bool hasPineapple)
        {
            Name = name;
            BasePrice = basePrice;
            DoughType = doughType;
            SauceType = sauceType;
            MozzarellaPortions = mozzarellaPortions;
            CheddarPortions = cheddarPortions;
            PepperoniPortions = pepperoniPortions;
            JalapenoPortions = jalapenoPortions;
            DefaultMozzarella = defaultMozzarella;
            DefaultCheddar = defaultCheddar;
            DefaultPepperoni = defaultPepperoni;
            DefaultJalapeno = defaultJalapeno;
            HasChicken = hasChicken;
            HasMushrooms = hasMushrooms;
            HasTomatoes = hasTomatoes;
            HasBasil = hasBasil;
            HasHam = hasHam;
            HasPineapple = hasPineapple;
        }

        public int GetTotalPrice()
        {
            int price = BasePrice;
            price += MozzarellaPortions * 60;
            price += CheddarPortions * 70;
            price += PepperoniPortions * 80;
            price += JalapenoPortions * 50;
            return price;
        }

        public string GetToppingsString()
        {
            List<string> toppings = new List<string>();
            int extraMozz = MozzarellaPortions - DefaultMozzarella;
            int extraChed = CheddarPortions - DefaultCheddar;
            int extraPepp = PepperoniPortions - DefaultPepperoni;
            int extraJal = JalapenoPortions - DefaultJalapeno;

            if (extraMozz > 0) toppings.Add($"Моцц +{extraMozz}");
            if (extraChed > 0) toppings.Add($"Чедд +{extraChed}");
            if (extraPepp > 0) toppings.Add($"Пепп +{extraPepp}");
            if (extraJal > 0) toppings.Add($"Хал +{extraJal}");

            return toppings.Count > 0 ? string.Join(" | ", toppings) : "Без топпингов";
        }

        public bool IsSameAs(Pizza other)
        {
            return this.Name == other.Name &&
                   this.DoughType == other.DoughType &&
                   this.SauceType == other.SauceType &&
                   this.MozzarellaPortions == other.MozzarellaPortions &&
                   this.CheddarPortions == other.CheddarPortions &&
                   this.PepperoniPortions == other.PepperoniPortions &&
                   this.JalapenoPortions == other.JalapenoPortions &&
                   this.HasChicken == other.HasChicken &&
                   this.HasMushrooms == other.HasMushrooms &&
                   this.HasTomatoes == other.HasTomatoes &&
                   this.HasBasil == other.HasBasil &&
                   this.HasHam == other.HasHam &&
                   this.HasPineapple == other.HasPineapple;
        }

        public override string ToString()
        {
            return $"{Name} [{GetTotalPrice()} руб.]";
        }
    }
}