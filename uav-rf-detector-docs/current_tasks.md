# Service
- [ ] Отрефакторить grpc-stream
- [ ] Разобраться в том, за что отвечает каждая строчка в реализации 
- [ ] Add https://github.com/gabime/spdlog to CMake.
begin-snippet: server-side-helloworld
- [ ] MVP реализация GRPC протокола:
    - [ ] Отправка обнаружений с частотой, протоколом, датой обнаружения в виде GRPC потока.
    - [ ] Heartbeat (клиент отправляет серверу оповощение о том, что он еще жив).
- [ ] Добавить поддержку async Postgres для БД обнаружений.
- [ ] Добавить поддержку Gitlab CI (build, test, run, pack)
- [x] Добавить сборку с помощью Conan, CMakeConan

# Client
- [ ] Отрефакторить grpc-stream
- [ ] Add https://github.com/gabime/spdlog to CMake.
- [ ] MVP реализация GRPC протокола.
- [ ] Добавить поддержку БД SQLite для журналирования событий. 
- [ ] Убрать использование шрифта из билд директории (закинуть в CMake при сборке).
- [ ] Развернуть столбцы таблицы в весь компонент по-умолчанию.
- [ ] Добавить шейдерный эффект для запущенного поиска.
- [ ] Добавить шейдерный эффект для активной цели.
- [ ] Добавить поддержку Gitlab CI (build, test, run, pack)
- [x] Добавить сборку с помощью Conan, CMakeConan
- [x] Add https://github.com/Tradias/asio-grpc/tree/master
- [x] asio-grpc server + client hello world 
- [x] Add protobuf & grpc to CMake.
- [x] Hello world jrpc server + client
- [x] Add Boost to CMake.

# ML
- [ ] Сделать FFT датасета CardRF по гайду: https://stackoverflow.com/questions/64176002/fast-fourier-transform-of-csv-data
- [ ] Обучить модель по статье: "RF-Enabled Deep-Learning-Assisted Drone Detection and
Identification: An End-to-End Approach"
