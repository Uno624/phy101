![Screenshot 2025-03-28 155803](https://github.com/user-attachments/assets/dd13fec4-c6e5-4747-83f2-dadfa637a1a8)

ดันขึ้นดันลงได้ ดันขึ้นเป็นการย้อนกลับ(BUTTON_R) ดันลงเป็นการเลื่อนmenu(BUTTON_L) กดเป็นไกลการเหลือ(BUTTON_SELECT)
![20250414_175702](https://github.com/user-attachments/assets/d0c5231b-9c5f-4d25-949b-380a70bc7f18)

สมการ High's taget 3จุด เส้นตรงกลางสามเหลียมคือ common พายามให้องศาใกล้ 0 มากสุด เพราะมันจะเก็บระยาเราจากเป้าหมาย ถ้ามุมมันมีความคาคเคลือนไปปรับ offset ได้ที่ dedug sensor
![Screenshot 2025-04-14 201831](https://github.com/user-attachments/assets/429babe4-2b87-4b79-b036-8a8eada19fbe)

สมการ how fase boy 

![Screenshot 2025-04-14 203343](https://github.com/user-attachments/assets/4355a980-b48a-4424-b2f8-f6d54c9a99fb)

pitch0tan เป็นการใช้สมการ atan2(mpu.getAccY(), sqrt(mpu.getAccX() * mpu.getAccX() + mpu.getAccZ() * mpu.getAccZ())) * RAD_TO_DEG เอาค่าอัตราเร่งมาหาค่ามุมองศาแต่หมุนได้ไม่เกิน 75-80องศา pitchget เป็นค่าองศาที่มาจาก LIBARRY <MPU6050_light.h> อ่านมุมได้เกิน 75 แต่ค่าคาคเคลือนตอนเวลาเขย่าหรือหันเร็วๆ (เราใช้pitch0tan)
![20250414_175904](https://github.com/user-attachments/assets/2d7a3707-82d2-4c6f-b73d-698c216cfd6c)

ปรับ offset ด้วยการกดไกล (ได้ทุกหน้าในmenuoffset)
![20250414_180026](https://github.com/user-attachments/assets/d73b1946-47d4-40dd-87ab-f6aea7c9565a)

****************************************************************************************************************
รูปภาพ
https://photos.app.goo.gl/7rdrjzsT8SxSt7gi9
