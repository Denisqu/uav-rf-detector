# Backend-core
- [ ] Реализовать masterWriter. Должен отправлять данные из потока "detector", перенаправляи все обнаружения/спектр клиентам gRPC.
- [ ] Удалять клиента из DetectorService map'ы после дисконнекта.
- [ ] Прочитать полностью overview boost.asio.
- [ ] Написать для каждого класса brief на русском языке.
- [ ] Add gtest to CMake.
- [ ] Написать несколько юнит-тестов.
- [ ] Разобраться в том, за что отвечает каждая строчка в реализации 
begin-snippet: server-side-helloworld
- [ ] MVP реализация GRPC протокола:
    - [ ] Отправка обнаружений с частотой, протоколом, датой обнаружения в виде GRPC потока.
    - [ ] Heartbeat (клиент отправляет серверу оповощение о том, что он еще жив).
- [ ] Убрать постоянные аллокации, использовать свой аллокатор.
- [ ] Добавить поддержку async Postgres для БД обнаружений.
- [ ] Добавить поддержку Gitlab CI (build, test, run, pack)
- [x] Добавить отправку Detection без запроса, через случайные интервалы времени.
- [x] Добавить uuid metadata в клиента, на стороне сервиса вести учёт клиентов в "libcuckoo/cuckoohash_map.hh". Добавить KeepAlive разрыв.
- [x] Логировать каждый разрыв соединения на сервере.
- [x] Превратить логгер в async logger
- [x] Если билд запущен в дебаг-режиме, то отображать source-function-line: https://github.com/gabime/spdlog/issues/318#issuecomment-989708120
- [x] Написать свой логгер с log_cat, как здесь: https://github.com/gabime/spdlog/issues/154 (api как у `Q_LOGGING_CATEGORY`)
- [x] Add https://github.com/gabime/spdlog to CMake.
- [x] Добавить сборку с помощью Conan, CMakeConan

# Backend-detector
- [ ] Реализовать MVP detector, который отправляет обнаружения для Backend-core по asio-tcp-каналу, инкапсулируя данные в Protobuf.

# Client
- [ ] Add https://github.com/gabime/spdlog to CMake.
- [ ] Перейти от grpc-asio к grpc-async-callback.
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
- [ ] Разобрать хорошую статью про ML + Signal proccessing: "https://ataspinar.com/2018/04/04/machine-learning-with-signal-processing-techniques/".
