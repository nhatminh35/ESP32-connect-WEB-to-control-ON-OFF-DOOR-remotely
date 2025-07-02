// Khởi tạo các thông số
let btn1 = document.querySelector('#btn1'); // Bật quạt

let btn3 = document.querySelector('#btn3'); // Bật đèn

let btn5 = document.querySelector('#btn5'); // Bật đèn

let btn7 = document.querySelector('#btn7'); // Bật đèn


document.addEventListener('DOMContentLoaded', () => {
    const btn_CUA_1_ON = document.querySelector('#btn_CUA_1_ON');
    const btn_CUA_2_ON = document.querySelector('#btn_CUA_2_ON');
    const CUA_1 = document.querySelector('#CUA_1');
    const CUA_2 = document.querySelector('#CUA_2');

    btn_CUA_1_ON.addEventListener('click', () => {
        CUA_1.src = "img/exit.png";
        firebase.database().ref("CUA_1").set({device1: 1});
        // Sau 5 giây thì tắt đèn
        setTimeout(() => {
            CUA_1.src = "img/lock.png";
            firebase.database().ref("CUA_1").set({device1: 0});
        }, 5000); // 5000 ms = 5 giây
    });

    btn_CUA_2_ON.addEventListener('click', () => {
        CUA_2.src = "img/exit.png";
        firebase.database().ref("CUA_2").set({device1: 1});
        // Sau 5 giây thì tắt quạt
        setTimeout(() => {
            CUA_2.src = "img/lock.png";
            firebase.database().ref("CUA_2").set({device1: 0});
        }, 5000);
    });
});

let CUA_1 = document.querySelector('#CUA_1');
let CUA_2 = document.querySelector('#CUA_2');


let btn11 = document.querySelector('#CLEAR_STORAGE'); // Tắt đèn
let btn13 = document.querySelector('#btn13');
let btn14 = document.querySelector('#btn14');
let btn15 = document.querySelector('#btn15');
let btn16 = document.querySelector('#btn16');
let state1 = localStorage.getItem("state1") ? parseInt(localStorage.getItem("state1")) : 0;
let state2 = localStorage.getItem("state2") ? parseInt(localStorage.getItem("state2")) : 0;
let state3 = localStorage.getItem("state3") ? parseInt(localStorage.getItem("state3")) : 0;
let state4 = localStorage.getItem("state4") ? parseInt(localStorage.getItem("state4")) : 0;
let state5 = localStorage.getItem("state5") ? parseInt(localStorage.getItem("state5")) : 0;
let device1Img = document.querySelector('#device1'); // Ảnh quạt  /* là cái id cho biết thông tin ảnh
let device2Img = document.querySelector('#device2'); // Ảnh đèn
let device3Img = document.querySelector('#device3'); // Ảnh đèn
let device4Img = document.querySelector('#device4'); // Ảnh đèn

let device5Img = document.querySelector('#CUA_1'); // Ảnh đèn
let device6Img = document.querySelector('#CUA_2'); // Ảnh đèn
// Functions nút bấm


btn1.addEventListener('click', () => {
    state1 = state1 == 0 ? 1 : 0; // nếu đang 0 thì thành 1, ngược lại 0
    localStorage.setItem("state1", state1); // lưu trạng thái mới vào LocalStorage
    UpdateImage1();  

    // Cập nhật Firebase
    
    firebase.database().ref("thietbi1").set({device1: state1});
});

btn3.addEventListener('click', () => {
    state2 = state2 == 0 ? 1 : 0; // nếu đang 0 thì thành 1, ngược lại 0
    localStorage.setItem("state2", state2); // lưu trạng thái mới vào LocalStorage
    UpdateImage2();  

    // Cập nhật Firebase
    
    firebase.database().ref("thietbi2").set({device2: state2});
});




btn5.addEventListener('click', () => {
    state3 = state3 == 0 ? 1 : 0; // nếu đang 0 thì thành 1, ngược lại 0
    localStorage.setItem("state3", state3); // lưu trạng thái mới vào LocalStorage
    UpdateImage3();  

    // Cập nhật Firebase
    
    firebase.database().ref("thietbi3").set({device3: state3});
});



btn7.addEventListener('click', () => {
    state4 = state4 == 0 ? 1 : 0; // nếu đang 0 thì thành 1, ngược lại 0
    localStorage.setItem("state4", state4); // lưu trạng thái mới vào LocalStorage
    UpdateImage4();  

    // Cập nhật Firebase
    
    firebase.database().ref("thietbi4").set({device4: state4});
});



btn9.addEventListener('click', () => {
    state5 = 1;
    firebase.database().ref("thietbi1").set({device1: 1})
    firebase.database().ref("thietbi2").set({device2: 1})
    firebase.database().ref("thietbi3").set({device3: 1})
    firebase.database().ref("thietbi4").set({device4: 1})
 
    localStorage.setItem("state5", state5); // Lưu vào LocalStorage
    UpdateALL();
});


btn10.addEventListener('click', () => {
    state5 = 0;
    firebase.database().ref("thietbi1").set({device1: 0})
    firebase.database().ref("thietbi2").set({device2: 0})
    firebase.database().ref("thietbi3").set({device3: 0})
    firebase.database().ref("thietbi4").set({device4: 0})
  
    localStorage.setItem("state5", state5); // Lưu vào LocalStorage
    UpdateALL();
});


btn11.addEventListener('click', () => {
    localStorage.removeItem("dataList");
    localStorage.removeItem("dataList_B"); 
    dataList = []; 
    dataList_B = [];
    let tableBody = document.querySelector("#dataTable tbody");
    tableBody.innerHTML = ""; 
});   
btn13.addEventListener('click', () => {
    console.log("Button đã được nhấn!");
    firebase.database().ref("FINGER_DATA").set({check1: 1});
});   


// Chờ DOM tải xong trước khi thực hiện hành động
document.addEventListener("DOMContentLoaded", function () {
    console.log("Script loaded, checking elements...");

    let fanImage = document.getElementById("fanImage");
    let speakerImage = document.getElementById("speakerImage");

    if (!fanImage || !speakerImage) {
        console.error("Không tìm thấy fanImage hoặc speakerImage");
    }

    // Gán sự kiện cho các nút bật/tắt
    document.getElementById("device20").addEventListener("click", turnOnFan);
    document.getElementById("device21").addEventListener("click", turnOffFan);
    document.getElementById("device22").addEventListener("click", turnOnSpeaker);
    document.getElementById("device23").addEventListener("click", turnOffSpeaker);
});

// Hàm bật/tắt quạt





function UpdateImage1(){
    if (state1 === 1) {
        device1.src = "img/lamp_on.png";
    } else {
        device1.src = "img/lamp_off.png";
    }

}
function UpdateImage2(){
    if (state2 === 1) {
        device2.src = "img/lamp_on.png";
    } else {
        device2.src = "img/lamp_off.png";
    }

}


function UpdateImage3(){
    if (state3 === 1) {
        device3.src = 'img/lamp_on.png'; // Đổi ảnh đèn bật
    } else {
        device3.src = 'img/lamp_off.png'; // Đổi ảnh đèn tắt
    }

}
function UpdateImage4(){
    if (state4 === 1) {
        
        device4.src = 'img/lamp_on.png'; // Đổi ảnh đèn bật
    } else {
        device4.src = 'img/lamp_off.png'; // Đổi ảnh đèn tắt
    }

}
function UpdateALL(){
    if (state5 == 1) {
        state1 = 1;
        state2 = 1;
        state3 = 1;
        state4 = 1;
     
    } else if(state5 == 0) {
        state1 = 0;
        state2 = 0;
        state3 = 0;
        state4 = 0;
       
    }
    state5 = 3;
    localStorage.setItem("state5", state5); // Lưu vào LocalStorage
    localStorage.setItem("state1", state1); // Lưu vào LocalStorage
    localStorage.setItem("state2", state2); // Lưu vào LocalStorage
    localStorage.setItem("state3", state3); // Lưu vào LocalStorage
    localStorage.setItem("state4", state4); // Lưu vào LocalStorage
    UpdateImage1();
    UpdateImage2();
    UpdateImage3();
    UpdateImage4();

}
/*      localStorage.removeItem('ID1List');
        localStorage.removeItem('ID2List'); */
 function createButtonMatrix(rows, cols) {
  const container = document.getElementById('buttonGrid');
  container.innerHTML = '';  // Xóa nội dung cũ nếu có
  container.style.display = 'grid';
  container.style.gridTemplateColumns = `repeat(${cols}, 50px)`; // 50px cho mỗi cột
  container.style.gridGap = '5px';

  for (let i = 0; i < rows * cols; i++) {
    const btn = document.createElement('button');
    btn.textContent = i + 1;
    btn.style.height = '50px';
    btn.style.width = '50px';
    btn.classList.add('matrix-button');  // Bạn có thể style thêm qua CSS nếu muốn
    btn.addEventListener('click', () => {
      document.getElementById('inputNumber1').value = i + 1;
    });
    container.appendChild(btn);
  }
}

// Gọi hàm để tạo ma trận 3x3 nút
createButtonMatrix(3, 3);
