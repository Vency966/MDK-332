public class Main {
    public static void main(String[] args) {
        // --- 3 теста с Assert ---
        Animal lion = new Lion("Симба", 5, 190.5, 1, 10, 50.0);
        assert lion.getName().equals("Симба") : "Тест 1: имя неверное";
        assert lion.getTotalCost() == 500.0 : "Тест 2: расчет стоимости неверный";

        Zoo zoo = new Zoo();
        zoo.addAnimal(lion);
        zoo.addAnimal(new Python("Каа", 3, 20.0, 2, 5, 30.0));
        assert zoo.getDailyCostAll() == 80.0 : "Тест 3: сумма за день неверная";

        System.out.println("✅ Все тесты пройдены!\n");

        // --- Демонстрация ---
        System.out.println("Все животные:");
        zoo.printAll();
        System.out.println("\nРасходы за день: " + zoo.getDailyCostAll());
        System.out.println("Расходы за весь период: " + zoo.getTotalCostAll());

        zoo.removeAnimal("Симба");
        System.out.println("\nПосле удаления Симбы:");
        zoo.printAll();
    }
}