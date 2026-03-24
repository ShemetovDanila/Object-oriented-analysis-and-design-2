namespace WITH_pattern
{
    public class PizzaDirector
    {
        public void MakeMargarita(IPizzaBuilder b)
        {
            b.Reset()
             .SetName("Маргарита")
             .SetBasePrice(450)
             .SetIsCustom(false)
             .SetDough("Классическое")
             .SetSauce("Томатный")
             .AddTopping("Моцарелла")
             .AddTopping("Базилик")
             .AddTopping("Помидоры");
        }

        public void MakePepperoni(IPizzaBuilder b)
        {
            b.Reset()
             .SetName("Пепперони")
             .SetBasePrice(550)
             .SetIsCustom(false)
             .SetDough("Классическое")
             .SetSauce("Томатный")
             .AddTopping("Пепперони")
             .AddTopping("Моцарелла");
        }

        public void MakeCountry(IPizzaBuilder b)
        {
            b.Reset()
             .SetName("Кантри")
             .SetBasePrice(500)
             .SetIsCustom(false)
             .SetDough("Толстое")
             .SetSauce("Барбекю")
             .AddTopping("Бекон")
             .AddTopping("Лук")
             .AddTopping("Грибы");
        }

        public void MakeHawaiian(IPizzaBuilder b)
        {
            b.Reset()
             .SetName("Гавайская")
             .SetBasePrice(520)
             .SetIsCustom(false)
             .SetDough("Классическое")
             .SetSauce("Томатный")
             .AddTopping("Курица")
             .AddTopping("Ананас")
             .AddTopping("Моцарелла");
        }

        public void MakeChicken(IPizzaBuilder b)
        {
            b.Reset()
             .SetName("Куриная")
             .SetBasePrice(530)
             .SetIsCustom(false)
             .SetDough("Тонкое")
             .SetSauce("Сливочный")
             .AddTopping("Курица")
             .AddTopping("Пармезан")
             .AddTopping("Чеснок");
        }

        public void MakeCustomPizza(IPizzaBuilder b)
        {
            b.Reset()
             .SetName("Конструктор")
             .SetBasePrice(200)
             .SetIsCustom(true)
             .SetDough("Классическое")
             .SetSauce("Томатный");
        }
    }
}