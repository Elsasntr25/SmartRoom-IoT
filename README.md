# SmartRoom-IoT

## Deskripsi Proyek

SmartRoom-IoT adalah proyek Internet of Things (IoT) yang dirancang untuk mengotomatisasi dan memantau ruangan pintar. Proyek ini menggunakan mikrokontroler seperti Arduino untuk mengintegrasikan sensor dan aktuator, memungkinkan kontrol jarak jauh melalui koneksi internet atau aplikasi mobile.

## Fitur Utama

- **Monitoring Sensor**: Mengukur parameter lingkungan seperti suhu, kelembaban, cahaya, dan gerakan.
- **Kontrol Aktuator**: Mengontrol perangkat seperti lampu, kipas, dan pintu otomatis.
- **Koneksi IoT**: Terhubung ke platform cloud untuk data logging dan kontrol remote.
- **Antarmuka Pengguna**: Aplikasi web atau mobile untuk interaksi pengguna.

## Hardware yang Dibutuhkan

- Mikrokontroler: Arduino Uno, ESP32, atau Raspberry Pi
- Sensor:
  - DHT11/DHT22 untuk suhu dan kelembaban
  - LDR untuk intensitas cahaya
  - PIR sensor untuk deteksi gerakan
- Aktuator:
  - Relay module untuk kontrol lampu/kipas
  - Servo motor untuk pintu otomatis
- Modul Komunikasi:
  - WiFi module (ESP8266) atau Ethernet shield
  - MQTT broker untuk komunikasi IoT

## Software yang Dibutuhkan

- Arduino IDE untuk upload kode ke mikrokontroler
- Library Arduino:
  - DHT sensor library
  - WiFi library
  - PubSubClient untuk MQTT
- Platform IoT: ThingSpeak, Adafruit IO, atau Blynk untuk dashboard

## Instalasi

1. **Persiapan Hardware**:
   - Hubungkan sensor dan aktuator ke pin yang sesuai pada mikrokontroler sesuai dengan skema wiring.

2. **Upload Kode**:
   - Buka file `sketch_dec16a.ino` di Arduino IDE.
   - Pilih board dan port yang benar.
   - Upload kode ke mikrokontroler.

3. **Konfigurasi Jaringan**:
   - Edit kredensial WiFi dan MQTT di kode.
   - Pastikan mikrokontroler terhubung ke internet.

4. **Setup Dashboard**:
   - Buat akun di platform IoT pilihan.
   - Konfigurasi channel atau dashboard untuk menerima data dari perangkat.

## Penggunaan

1. Nyalakan perangkat IoT.
2. Perangkat akan mulai mengirim data sensor ke cloud.
3. Gunakan aplikasi atau dashboard untuk memantau dan mengontrol perangkat.
4. Contoh:
   - Jika suhu > 30°C, aktifkan kipas.
   - Jika gerakan terdeteksi, nyalakan lampu.

## Kode Utama

Kode utama terletak di `sketch_dec16a.ino`. Kode ini mencakup:

- Inisialisasi sensor dan aktuator.
- Loop utama untuk membaca sensor dan mengirim data.
- Handler untuk kontrol remote via MQTT.


## Lisensi

Proyek ini menggunakan lisensi MIT. Lihat file LICENSE untuk detail lebih lanjut.

