# Service

- [ ] Если билд запущен в дебаг-режиме, то отображать source-function-line
- [ ] Превратить логгер в async logger
- [ ] Добавить uuid metadata в клиента, на стороне сервиса вести учёт клиентов в "libcuckoo/cuckoohash_map.hh". Добавить KeepAlive Response.
- [ ] Добавить отправку Detection без запроса, через случайные интервалы времени.
- [ ] Add gtest to CMake.
- [ ] Написать несколько юнит-тестов.
- [ ] Понять что это за код: 
```cpp
namespace service
{

template<auto RequestRPC>
using AwaitableServerRPC = boost::asio::use_awaitable_t<>::as_default_on_t<agrpc::ServerRPC<RequestRPC>>;

using DetectionServiceRPC = AwaitableServerRPC<&rfdetector::DetectionService::AsyncService::RequestMainStream>;

}
```
- [ ] Отрефакторить grpc-stream
- [ ] Разобраться в том, за что отвечает каждая строчка в реализации 
begin-snippet: server-side-helloworld
- [ ] MVP реализация GRPC протокола:
    - [ ] Отправка обнаружений с частотой, протоколом, датой обнаружения в виде GRPC потока.
    - [ ] Heartbeat (клиент отправляет серверу оповощение о том, что он еще жив).
- [ ] Добавить поддержку async Postgres для БД обнаружений.
- [ ] Добавить поддержку Gitlab CI (build, test, run, pack)
- [x] Написать свой логгер с log_cat, как здесь: https://github.com/gabime/spdlog/issues/154 (api как у `Q_LOGGING_CATEGORY`)
- [x] Add https://github.com/gabime/spdlog to CMake.
- [x] Добавить сборку с помощью Conan, CMakeConan

# Client
- [ ] Add https://github.com/gabime/spdlog to CMake.
- [ ] Отрефакторить grpc-stream
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
