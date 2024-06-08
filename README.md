# PetFeeder

## Описание
Данное устройство состоит из двух элементов: самой кормушки и ошейника для питомца. 
Ошейник представляет из себя BLE device, сигнал с которого считывает кормушка. Когда питомец с ошейником находится на достаточно близком расстоянии от устройства и/или подошло время кормления, происходит подача еды. 
Светодиод сигнализирует о процессе выдачи корма.
 </br>

## Инструкция
- Нажмите на кнопку 1 раз, чтобы кормушка выдала корм вне очереди. Светодиод будет гореть пока выдаётся корм. Когда выдача закончится светодиод погаснет.
- Нажмите кнопку и удерживайте её, чтобы задать размер порции. В это время будет гореть светодиод. Отпустите кнопку, когда высыпится столько корма, сколько Вам нужно. Светодиод погаснет.
- Задайте расписание кормежки через Телеграмм Бота.
- Привяжите ошейник к кормушке через Телеграмм Бота.
- Можно начать внеочередную кормежку через Телеграмм Бота.

  В файле Collar.ino находится прошивка для ошейника. В файле PetFeeder.ino находится прошивка для кормушки.

## Демонстрация
[https://youtu.be/il3nEXnl1cY](https://youtu.be/il3nEXnl1cY)

## Схема
[demonstrations/scheme.png](demonstrations/scheme.png)

## API:
### Добавление нового пользователя
```/add_user```

+ Request
```
{
  "user": "string"
}
```

+ Response
```
{
  "user": "string",
  "id": 0
}
```

### Список всех пользователей
```/users_all```

+ Response
```
[
  {
  "user": "string",
  "id": 0
  }
]
```

### Инициализация пользователя
```/user_by_chat_id```

+ Request
```
{
  "user": "string"
}
```

+ Response
```
{
  "message": "Exist",
  "id": 0
}
```

+ Response
```
{
  "message": "None"
}
```

### Добавление новой кормушки
```/add_feeder```

+ Request
```
{
  "user_id": 0,
  "feeder": "string"
}
```

+ Response
```
{
  "user_id": 0,
  "feeder": "string",
  "id": 0
}
```

### Список всех кормушек
```/feeders_all```

+ Response
```
[
  {
    "feeder": "string",
    "user_id": 0,
    "id": 0
  }
]
```

### Инициализация кормушки
```/feeder_by_user_id```

+ Request
```
{
  "user_id": 0,
  "feeder": "string"
}
```

+ Response
```
{
  "message": "Exist",
  "id": 0
}
```

+ Response
```
{
  "message": "None"
}
```

### Получение id кормушки
```/get_feeder_id```

+ Request
```
{
  "feeder": "string"
}
```

+ Response
```
{
  "feeder_id": 0
}
```

### Список кормушек пользователя
```/feeders_by_user```

+ Request
```
{
  "user": "string"
}
```

+ Response
```
[
  "string"
]
```

### Добавление нового ошейника
```/add_collar```

+ Request
```
{
  "feeder_id": 0,
  "collar": "string"
}
```

+ Response
```
{
  "feeder_id": 0,
  "collar": "string",
  "id": 0
}
```

### Список всех ошейников
```/collars_all```

+ Response
```
[
{
  "feeder_id": 0,
  "collar": "string",
  "id": 0
}
]
```

### Список всех ошейников
```/collar_by_feeder_id```

+ Request
```
{
  "feeder_id": 0,
  "collar": "string"
}
```

+ Response
```
{
  "message": "Exist",
  "id": 0
}
```

+ Response
```
{
  "message": "None"
}
```

## История
### Первая пробная сборка c Arduino:
[demonstrations/first_build.jpg](demonstrations/first_build.jpg)

### Вторая пробная сборка с esp8266:
[demonstrations/build_with_esp8266.jpg](demonstrations/build_with_esp8266.jpg)

### Итоговая сборка с esp32 (убрали bluetooth-модуль):
[demonstrations/build_with_esp32.jpg](demonstrations/build_with_esp32.jpg)

### Установка всех элементов в модели:
[demonstrations/finish_build.jpg](demonstrations/finish_build.jpg)
