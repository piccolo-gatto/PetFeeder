# PetFeeder

## Описание
Данное устройство состоит из двух элементов: самой кормушки и ошейника для питомца. 
Ошейник имеет в себе датчик, сигнал с которого считывает кормушка. Когда питомец с ошейником находится на достаточно близком расстоянии от устройства, происходит подача еды. </br>


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
