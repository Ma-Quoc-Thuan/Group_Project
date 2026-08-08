 Cổng Quản Lý Sinh Viên & Đào Tạo

## 📌 Giới Thiệu (About the Project)
 (Student Information Management System) là một ứng dụng desktop được xây dựng trên nền tảng **C++** và **Qt Framework**[cite: 1, 3]. Hệ thống được thiết kế tối ưu nhằm phục vụ công tác quản lý sinh viên, giảng viên và đào tạo tín chỉ, phù hợp để ứng dụng hoặc làm đồ án môn học chuyên ngành C++ tại Khoa Công nghệ Thông tin, trường Đại học Khoa học Tự nhiên (HCMUS). 

Chương trình áp dụng các nguyên lý lập trình hướng đối tượng (OOP) và sử dụng các Design Pattern chuẩn như **Singleton** (cho DatabaseManager)[cite: 3] và **Factory Method** (cho UserFactory)[cite: 8].

---

## ✨ Tính Năng Chính (Key Features)

### 1. Hệ Thống Đăng Nhập & Phân Quyền (Authentication)
* Hỗ trợ đăng nhập với hai vai trò riêng biệt: Quản trị viên (Admin) và Sinh viên (Student) thông qua mô hình Factory Method[cite: 6, 8].
* Cho phép người dùng thay đổi và cập nhật mật khẩu an toàn[cite: 2].

### 2. Dành Cho Quản Trị Viên (Admin Dashboard)
* **Quản lý dữ liệu sinh viên:** Thêm, sửa, xóa thông tin sinh viên (MSSV, Họ tên, Ngày sinh, Quê quán, Lớp)[cite: 1, 4].
* **Nhập liệu nhanh (Import):** Hỗ trợ nhập danh sách sinh viên hàng loạt thông qua tệp `sinhvien.csv`[cite: 1].
* **Quản lý đào tạo:** Tạo học phần mới (Mã môn, Tên môn, Số tín chỉ) và phân bổ lịch học (Thứ, Tiết, Phòng học) cho từng lớp[cite: 1].
* **Thống kê tổng quan:** Hiển thị trực quan tổng số sinh viên, GPA trung bình toàn trường và tỷ lệ sinh viên Giỏi/Xuất sắc[cite: 1].

### 3. Quản Lý Điểm & Kết Quả Học Tập (Grade Management)
* Đăng ký và hủy học phần cho từng sinh viên[cite: 5].
* Nhập và quản lý điểm số: Điểm giữa kỳ (GK) và Điểm cuối kỳ (CK)[cite: 5].
* Tự động tính toán điểm trung bình từng môn và cập nhật GPA tích lũy của sinh viên (hệ số 10.0)[cite: 5].

---

## 🚀 Cài Đặt & Chạy Môi Trường (Installation & Usage)

### Yêu cầu hệ thống (Prerequisites)
* Trình biên dịch C++ (Hỗ trợ C++11 trở lên).
* **Qt IDE** (Qt Creator) và bộ thư viện Qt Widgets.

### Cách chạy chương trình
1. Clone repository này về máy tính.
2. Mở file project `.pro` (nếu có) bằng **Qt Creator**.
3. Build và Run chương trình.
4. Để test tính năng nhập dữ liệu hàng loạt ở Tab Admin, hãy tải lên file `sinhvien.csv` đính kèm trong thư mục dự án[cite: 1].

---

## 🔐 Tài Khoản Mặc Định (Default Credentials)

Sử dụng các tài khoản giả lập sau để truy cập hệ thống ngay lần đầu tiên:

* **Tài khoản Admin:**
  * **Username:** `admin`[cite: 6]
  * **Password:** `admin`[cite: 6]

* **Tài khoản Sinh viên (Dữ liệu mẫu):**
  * **Username:** Nhập MSSV của sinh viên bất kỳ (VD: `20230001` hoặc MSSV được tạo trong cơ sở dữ liệu)[cite: 6].
  * **Password:** Mật khẩu mặc định là `123456` (Sinh viên có thể đổi sau khi đăng nhập)[cite: 6].

