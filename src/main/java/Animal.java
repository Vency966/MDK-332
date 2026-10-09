public abstract class Animal {
    protected String name;
    protected int age;
    protected double weight;
    protected int enclosureNumber;
    protected int daysInZoo;
    protected double dailyFoodCost;

    public Animal(String name, int age, double weight, int enclosureNumber, int daysInZoo, double dailyFoodCost) {
        if (age < 0) throw new IllegalArgumentException("Возраст не может быть отрицательным");
        if (weight <= 0) throw new IllegalArgumentException("Вес должен быть положительным");
        if (daysInZoo < 0) throw new IllegalArgumentException("Дней в зоопарке не может быть отрицательно");
        if (dailyFoodCost < 0) throw new IllegalArgumentException("Расход не может быть отрицательным");

        this.name = name;
        this.age = age;
        this.weight = weight;
        this.enclosureNumber = enclosureNumber;
        this.daysInZoo = daysInZoo;
        this.dailyFoodCost = dailyFoodCost;
    }

    public String getName() { return name; }
    public double getDailyFoodCost() { return dailyFoodCost; }
    public double getTotalCost() { return dailyFoodCost * daysInZoo; }

    @Override
    public String toString() {
        return String.format("%s | Кличка: %s | Возраст: %d | Вес: %.1f | Вольер: %d | Дней: %d | Еда/день: %.2f",
                getClass().getSimpleName(), name, age, weight, enclosureNumber, daysInZoo, dailyFoodCost);
    }
}