function makeid() {
  var text = "";
  //Chuoi so ngau nhien
  var possible =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  for (var i = 0; i < 5; i++)
    //gan gia tri cho text su dung math.floor de lam tron ham random
    text += possible.charAt(Math.floor(Math.random() * possible.length));
  return text;
}

//tao 1 phien may khach
var client = new Paho.MQTT.Client("broker.hivemq.com", 8000, makeid());
//Đặt các trình xử lý gọi lại
client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

//Tạo đối tượng options
var options = {
  useSSL: false,
  userName: "",
  password: "",
  onSuccess: onConnect,
  onFailure: doFail,
};

//Kết nối đến máy khách
client.connect(options);

function doFail(e) {
  console.log(e);
  // document.getElementById("icon_connect").style.color = "red";
}

function onConnect() {
  // window.alert("Chào mừng bạn đến với HyHome");
  // document.getElementById("icon_connect").style.color = "#00d084";
  // document.getElementById("title_connect").innerHTML = "Đã Kết Nối";
  console.log("Kết nối thành công đến broker");
  client.subscribe("Esp32/sensors");
}

//Gọi hàm onConnectionLost khi bị mất kết nối
function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0) {
    console.log(responseObject.errorMessage);
  }
}

async function onMessageArrived(message) {
  console.log(message.destinationName + ":" + message.payloadString);
  var dataSever = message.payloadString;
  var dataJson = JSON.parse(dataSever);

  var num_temperature = dataJson.tempValue;
  document.getElementById("message_sensors_temperature").innerHTML =
    num_temperature + "°C";

  // var num_Ph = dataJson.pHValue;
  var num_Ph = dataJson.pHValue;
  document.getElementById("message_sensors_pH").innerHTML = num_Ph + "";

  // var num_tds = dataJson.TDSValue;
  var num_tds = dataJson.TDSValue;
  document.getElementById("message_sensors_TDS").innerHTML = num_tds + " PPM";

  var num_NTU = dataJson.turbidityValue;
  document.getElementById("message_sensors_NTU").innerHTML = num_NTU + " NTU";

  // So sánh thông báo chất lượng của nguồn ngước
  // So sánh chất lượng Temperature
  if (num_temperature <= 40) {
    document.getElementById("infor_temperature").innerHTML = "BÌNH THƯỜNG";
  }
  if (num_temperature > 40 && num_temperature <= 50) {
    document.getElementById("infor_temperature").innerHTML = "NƯỚC ẤM";
  }
  if (num_temperature > 50) {
    document.getElementById("infor_temperature").innerHTML = "NƯỚC NÓNG";
  }
  // So sánh chất lượng pH
  if (num_Ph <= 5.8) {
    document.getElementById("infor_pH").innerHTML = "Có Tính Axit";
  }
  if (num_Ph >= 6 && num_Ph <= 7.2) {
    document.getElementById("infor_pH").innerHTML = "BÌNH THƯỜNG";
  }

  if (num_Ph > 7.2) {
    document.getElementById("infor_pH").innerHTML = "CÓ TÍNH BAZO";
  }

  // So sánh chất lượng TDS
  if (num_tds <= 100) {
    document.getElementById("infor_TDS").innerHTML = "NƯỚC UỐNG";
  }
  if (num_tds > 10 && num_Ph <= 310) {
    document.getElementById("infor_TDS").innerHTML = "NƯỚC SINH HOẠT";
  }

  if (num_tds > 310) {
    document.getElementById("infor_TDS").innerHTML = "Ô NHIỄM";
  }
  // So sánh chất lượng Turbidity
  if (num_NTU <= 10) {
    document.getElementById("infor_NTU").innerHTML = "NƯỚC UỐNG";
  }
  if (num_NTU > 10 && num_Ph <= 50) {
    document.getElementById("infor_NTU").innerHTML = "NƯỚC SINH HOẠT";
  }

  if (num_NTU > 50) {
    document.getElementById("infor_NTU").innerHTML = "Ô NHIỄM";
  }

  adddata(num_temperature);
  adddata_pH(num_Ph);
  adddata_tds(num_tds);
  adddata_ntu(num_NTU);
}
//-------------------------------- Plot for temperature----------------------------------------------------
// function Chart_Temp() {
var MAX_DATA_SET_LENGTH = 15;
function reLoad() {
  adddata(num_temperature);
}
var canvas = document.getElementById("myChart");
var data = {
  datasets: [
    {
      label: "Temperature (°C)",
      data: [],
      backgroundColor: "rgba(54, 162, 235, 0.2)",
      borderColor: "rgba(255, 99, 132, 1)",
      fill: false,
      borderWidth: 4,
      fill: true,
      tension: 0.3,
      pointStyle: "circle",
      pointRadius: 5,
      pointHoverRadius: 10,
    },
    {
      // label: "Upload",
      // borderColor: "rgb(0, 115, 255)",
      // fill: false,
      // borderWidth: 3,
    },
  ],
};
var options = {
  scales: {
    yAxes: {
      min: 0,
      max: 100,
      title: {
        display: true,
        text: "Value",
      },
    },
    xAxes: [
      {
        type: "time",
        ticks: {
          maxTicksLimit: 5,
        },
        time: {
          unit: "second",
          displayFormats: {
            second: "HH:mm:ss",
          },
          tooltipFormat: "HH:mm:ss",
        },
      },
    ],
  },
  showLines: true,
};
var myChart = new Chart.Line(canvas, {
  data: data,
  options: options,
});

function adddata(download = NaN, upload = NaN, label = moment()) {
  var datasets = myChart.data.datasets;
  var labels = myChart.data.labels;
  var downloadDataSet = datasets[0].data;
  var uploadDataSet = datasets[1].data;

  var downloadDataLength = downloadDataSet.length;
  var uploadDataLength = uploadDataSet.length;

  // if upload/download's data set has more than MAX_DATA_SET_LENGTH entries,
  // remove the first one entry and push on a new data entry
  var didRemoveData = false;
  if (downloadDataLength > MAX_DATA_SET_LENGTH) {
    downloadDataSet.shift();
    didRemoveData = true;
  }

  if (uploadDataLength > MAX_DATA_SET_LENGTH) {
    uploadDataSet.shift();
    didRemoveData = true;
  }

  // if either download or upload data was removed, we also need to remove
  // the first label to keep the data from squeezing in.
  if (didRemoveData) {
    labels.shift();
  }

  labels.push(label);
  downloadDataSet.push(download);
  uploadDataSet.push(upload);
  myChart.update();
}
setInterval(reLoad, 500);
// -----------------------------------Plot for Ph Sensor----------------------------------------------------
function reLoadPh() {
  adddata_pH(num_Ph);
}
var canvas2 = document.getElementById("myChart2");
var datapH = {
  datasets: [
    {
      label: "Pondus Hydrogenii (PH)",
      data: [],
      backgroundColor: "rgba(54, 162, 232, 0.2)",
      borderColor: "rgba(255, 99, 12, 1)",
      borderWidth: 4,
      fill: true,
      tension: 0.3,
      pointStyle: "circle",
      pointRadius: 5,
      pointHoverRadius: 10,
    },
    {
      // label: "Upload",
      // borderColor: "rgb(0, 115, 255)",
      // fill: false,
      // borderWidth: 3,
    },
  ],
};

var myChart2 = new Chart.Line(canvas2, {
  data: datapH,
  options: options,
});

function adddata_pH(download = NaN, upload = NaN, label = moment()) {
  var datasets = myChart2.data.datasets;
  var labels = myChart2.data.labels;
  var downloadDataSet = datasets[0].data;
  var uploadDataSet = datasets[1].data;

  var downloadDataLength = downloadDataSet.length;
  var uploadDataLength = uploadDataSet.length;

  // if upload/download's data set has more than MAX_DATA_SET_LENGTH entries,
  // remove the first one entry and push on a new data entry
  var didRemoveData = false;
  if (downloadDataLength > MAX_DATA_SET_LENGTH) {
    downloadDataSet.shift();
    didRemoveData = true;
  }

  if (uploadDataLength > MAX_DATA_SET_LENGTH) {
    uploadDataSet.shift();
    didRemoveData = true;
  }

  // if either download or upload data was removed, we also need to remove
  // the first label to keep the data from squeezing in.
  if (didRemoveData) {
    labels.shift();
  }

  labels.push(label);
  downloadDataSet.push(download);
  uploadDataSet.push(upload);
  myChart2.update();
}
setInterval(reLoadPh, 500);
// }
// -----------------------------------Plot for TDS Sensor----------------------------------------------------
function reLoadtds() {
  adddata_tds(num_tds);
}
var canvas3 = document.getElementById("myChart3");
var datatds = {
  datasets: [
    {
      label: "Total Dissolved Solids (PPM)",
      data: [],
      backgroundColor: "rgba(54, 162, 232, 0.2)",
      borderColor: "rgba(255, 69, 732, 1)",
      borderWidth: 4,
      fill: true,
      tension: 0.3,
      pointStyle: "circle",
      pointRadius: 5,
      pointHoverRadius: 10,
    },
    {},
  ],
};
var myChart3 = new Chart.Line(canvas3, {
  data: datatds,
  options: options,
});

function adddata_tds(download = NaN, upload = NaN, label = moment()) {
  var datasets = myChart3.data.datasets;
  var labels = myChart3.data.labels;
  var downloadDataSet = datasets[0].data;
  var uploadDataSet = datasets[1].data;

  var downloadDataLength = downloadDataSet.length;
  var uploadDataLength = uploadDataSet.length;

  // if upload/download's data set has more than MAX_DATA_SET_LENGTH entries,
  // remove the first one entry and push on a new data entry
  var didRemoveData = false;
  if (downloadDataLength > MAX_DATA_SET_LENGTH) {
    downloadDataSet.shift();
    didRemoveData = true;
  }

  if (uploadDataLength > MAX_DATA_SET_LENGTH) {
    uploadDataSet.shift();
    didRemoveData = true;
  }

  // if either download or upload data was removed, we also need to remove
  // the first label to keep the data from squeezing in.
  if (didRemoveData) {
    labels.shift();
  }

  labels.push(label);
  downloadDataSet.push(download);
  uploadDataSet.push(upload);
  myChart3.update();
}
setInterval(reLoadtds, 500);
// -----------------------------------Plot for TURBIDITY Sensor----------------------------------------------------
function reLoadNTU() {
  adddata_ntu(num_NTU);
}
var canvas4 = document.getElementById("myChart4");
var data_ntu = {
  datasets: [
    {
      label: "Water Turbidity - NTU",
      data: [],
      backgroundColor: "rgba(54, 162, 235, 0.2)",
      borderColor: "rgba(255, 199, 122, 1)",
      borderWidth: 4,
      fill: true,
      tension: 0.3,
      pointStyle: "circle",
      pointRadius: 5,
      pointHoverRadius: 10,
    },
    {
      // label: "Temperature",
      // data: [],
      // backgroundColor: "rgba(54, 162, 235, 0.2)",
      // borderColor: "rgba(255, 99, 132, 1)",
      // borderWidth: 4,
      // fill: true,
      // tension: 0.3,
      // pointStyle: "circle",
      // pointRadius: 5,
      // pointHoverRadius: 10,
    },
  ],
};

var myChart4 = new Chart.Line(canvas4, {
  data: data_ntu,
  options: options,
});

function adddata_ntu(download = NaN, upload = NaN, label = moment()) {
  var datasets = myChart4.data.datasets;
  var labels = myChart4.data.labels;
  var downloadDataSet = datasets[0].data;
  var uploadDataSet = datasets[1].data;

  var downloadDataLength = downloadDataSet.length;
  var uploadDataLength = uploadDataSet.length;

  // if upload/download's data set has more than MAX_DATA_SET_LENGTH entries,
  // remove the first one entry and push on a new data entry
  var didRemoveData = false;
  if (downloadDataLength > MAX_DATA_SET_LENGTH) {
    downloadDataSet.shift();
    didRemoveData = true;
  }

  if (uploadDataLength > MAX_DATA_SET_LENGTH) {
    uploadDataSet.shift();
    didRemoveData = true;
  }

  // if either download or upload data was removed, we also need to remove
  // the first label to keep the data from squeezing in.
  if (didRemoveData) {
    labels.shift();
  }

  labels.push(label);
  downloadDataSet.push(download);
  uploadDataSet.push(upload);
  myChart4.update();
}
setInterval(reLoadNTU, 1000);
//Xuất bản 1 tin nhắn tới 1 topic nào đó
function public(topic, data) {
  message = new Paho.MQTT.Message(data);
  message.destinationName = topic;
  client.send(message);
}
