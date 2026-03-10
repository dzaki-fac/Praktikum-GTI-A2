Penugasan Praktikum Pertemuan 1
Nama : Dzaki Fathul 'Alim Cahyo
NIM : 24060124130103
Lab A2

1. Primitive Drawing
<img width="1143" height="911" alt="image" src="https://github.com/user-attachments/assets/d32ea1e5-7bc3-4714-baef-b410d5eff4d1" />


2. Titik
<img width="1149" height="910" alt="image" src="https://github.com/user-attachments/assets/a83a695f-a9fd-4b6b-812c-8784c3c76ff2" />


3. Garis
<img width="1146" height="909" alt="image" src="https://github.com/user-attachments/assets/778dfbbe-a4ba-4128-a9c9-97c6a41dc76f" />


4. Segitiga
<img width="1142" height="910" alt="image" src="https://github.com/user-attachments/assets/0c247bde-540a-4204-96b6-ea056b587c34" />


5. Persegi
<img width="1145" height="899" alt="image" src="https://github.com/user-attachments/assets/a69cb1c5-09b1-4cc4-be2e-773749189ff0" />


Jawaban Pertanyaan
1. Apa fungsi glColor3f(1.0f, 0.0f, 0.0f) dan apa fungsi huruf f di belakang valuenya, serta bagaimana gabungan warna untuk menghasilkan warna hijau menggunakan fungsi glColor3f?
Fungsi glColor3f(1.0f, 0.0f, 0.0f) digunakan untuk mengatur warna objek yang akan digambar di OpenGL menggunakan sistem warna RGB yaitu Red, Green, dan Blue dengan rentang nilai 0.0 sampai 1.0. Pada contoh tersebut nilai merah bernilai 1.0 sedangkan hijau dan biru bernilai 0.0 sehingga menghasilkan warna merah. Huruf f di belakang angka menunjukkan bahwa nilai tersebut bertipe data float. Untuk menghasilkan warna hijau menggunakan fungsi ini dapat digunakan glColor3f(0.0f, 1.0f, 0.0f) karena komponen hijau bernilai maksimum sementara merah dan biru bernilai nol.

2. Apakah di dalam OpenGL terdapat fungsi glColor4f dan jika ada warna apa saja yang ada di dalamnya?
Di dalam OpenGL terdapat fungsi glColor4f yang digunakan untuk menentukan warna menggunakan empat komponen yaitu Red, Green, Blue, dan Alpha. Tiga komponen pertama menentukan warna seperti pada RGB, sedangkan komponen Alpha berfungsi untuk mengatur tingkat transparansi objek. Nilai alpha 1.0 berarti objek tidak transparan, sedangkan nilai 0.0 berarti objek sepenuhnya transparan.

3. Apa fungsi glVertex3f(-0.10, -0.10, 0.00)?
Fungsi glVertex3f(-0.10, -0.10, 0.00) digunakan untuk menentukan posisi sebuah titik atau vertex dalam ruang tiga dimensi pada OpenGL dengan koordinat x, y, dan z. Pada contoh tersebut titik berada pada posisi x = -0.10, y = -0.10, dan z = 0.00 sehingga posisinya berada sedikit ke kiri dan ke bawah dari titik pusat koordinat.

4. Apa fungsi glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA)?
Fungsi glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA) digunakan untuk menentukan mode tampilan jendela OpenGL saat program dijalankan. GLUT_SINGLE menunjukkan bahwa program menggunakan single buffer untuk proses rendering, sedangkan GLUT_RGBA menunjukkan bahwa sistem warna yang digunakan adalah RGBA yang terdiri dari red, green, blue, dan alpha.

Penugasan Praktikum
<img width="1412" height="1119" alt="image" src="https://github.com/user-attachments/assets/13c8bde7-ecc6-42ad-a2bb-ae723b10d946" />


Penjelasan singkat mengenai berbedaan fungsi
1. GL_POINT
GL_POINT digunakan untuk menggambar titik tunggal pada layar. Setiap vertex yang diberikan akan ditampilkan sebagai satu titik. Jika ada 5 vertex, maka akan muncul 5 titik terpisah. Primitive ini biasanya digunakan untuk menampilkan koordinat titik atau dasar dari objek grafika.

2. GL_LINES
GL_LINES digunakan untuk menggambar garis lurus antara dua titik. Setiap dua vertex akan membentuk satu garis. Jika ada 4 vertex, maka akan terbentuk dua garis, yaitu vertex pertama ke kedua dan vertex ketiga ke keempat.

3. GL_LINE_STRIP
GL_LINE_STRIP digunakan untuk menggambar serangkaian garis yang saling terhubung. Setiap vertex baru akan membuat garis dari vertex sebelumnya ke vertex tersebut. Garis tidak akan kembali ke titik awal.

4. GL_LINE_LOOP
GL_LINE_LOOP hampir sama dengan GL_LINE_STRIP, tetapi garis terakhir akan otomatis terhubung kembali ke vertex pertama sehingga membentuk bentuk tertutup.

5. GL_TRIANGLE_FAN
GL_TRIANGLE_FAN digunakan untuk menggambar beberapa segitiga yang memiliki satu titik pusat yang sama. Vertex pertama menjadi pusat, lalu setiap dua vertex berikutnya akan membentuk segitiga bersama titik pusat tersebut.

6. GL_TRIANGLE_STRIP
GL_TRIANGLE_STRIP digunakan untuk membuat rangkaian segitiga yang saling berbagi sisi. Setiap vertex baru akan membentuk segitiga dengan dua vertex sebelumnya sehingga jumlah vertex yang dibutuhkan lebih sedikit.

7. GL_QUADS
GL_QUADS digunakan untuk menggambar bangun empat sisi. Setiap empat vertex akan membentuk satu quad atau persegi. Jika ada delapan vertex maka akan terbentuk dua quad.

8. GL_QUAD_STRIP
GL_QUAD_STRIP digunakan untuk membuat rangkaian quad yang saling terhubung. Setiap dua vertex baru akan membentuk quad bersama dua vertex sebelumnya sehingga membentuk permukaan yang panjang dan tersambung.
