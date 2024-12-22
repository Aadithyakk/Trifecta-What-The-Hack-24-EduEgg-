function startDictation() {
  if (window.hasOwnProperty("webkitSpeechRecognition")) {
    var recognition = new webkitSpeechRecognition();
    recognition.continuous = true;
    recognition.interimResults = false;
    recognition.lang = "en-US";
    recognition.start();

    recognition.onresult = function (e) {
      document.getElementById("user-input").value = e.results[0][0].transcript;
      recognition.stop();
    };

    recognition.onerror = function (e) {
      recognition.stop();
    };
  }
}

let barData = ["loading", "loading", "loading"];
let option1 = 0;
let option2 = 0;
let option3 = 0;

const ctx = document.getElementById("myChart");
const myChart = new Chart(ctx, {
  type: "bar",
  data: {
    labels: [barData[0], barData[1], barData[2]],
    datasets: [
      {
        label: "Number of Votes",
        data: [option1, option2, option3],
        backgroundColor: ["#ebdf4a", "#e787d6", "#17e2df"],
        borderWidth: 1,
      },
    ],
  },
  options: {
    scales: {
      x: {
        ticks: {
          font: {
            size: 30,
          },
        },
      },
      y: {
        beginAtZero: true,
      },
    },
  },
});

async function sendMessage() {
  const userInput = document.getElementById("user-input");
  const chatContainer = document.getElementById("chat-container");

  if (!userInput.value.trim()) return;

  const userMessage = document.createElement("div");
  userMessage.className = "message user-message";
  userMessage.textContent = userInput.value;
  chatContainer.appendChild(userMessage);

  chatContainer.scrollTop = chatContainer.scrollHeight;

  const response = await fetch("https://api.openai.com/v1/chat/completions", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      Authorization:
        "Bearer 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",
    },
    body: JSON.stringify({
      model: "gpt-3.5-turbo",
      messages: [
        {
          role: "system",
          content:
            "Your job is to help provide 3 points, there must be 3 points, based on what a teacher is teaching, each point should not be longer than 2 words. provide me the data in an array with only the enclosing square brackets, each element in the array must be a string",
        },
        // { role: "assistant", content: "" },
        { role: "user", content: userInput.value },
      ],
    }),
  });

  const data = await response.json();

  const botMessage = document.createElement("div");
  botMessage.className = "message bot-message";
  botMessage.textContent = data.choices[0].message.content;
  chatContainer.appendChild(botMessage);
  console.log(botMessage.textContent);

  barData = JSON.parse(botMessage.textContent);
  console.log(typeof barData);
  for (let i = 0; i < barData.length; i++) {
    console.log(barData[i]);
  }

  // Update the chart with new labels
  myChart.data.labels = barData;
  myChart.update();

  chatContainer.scrollTop = chatContainer.scrollHeight;

  userInput.value = "";
}

let buttonArray = [];
document.getElementById("getData").addEventListener("click", async () => {
  try {
    const response = await fetch("http://192.168.37.119/get_answer");
    if (!response.ok) throw new Error("Network response was not ok");
    const text = await response.text();
    console.log(text);
    buttonArray.push(text);
    if (text == barData[0]) {
      option1++;
    } else if (text == barData[1]) {
      option2++;
    } else {
      option3++;
    }
    console.log(option1, option2, option3);
    document.getElementById("response").innerText = text;
    // Update chart data
    myChart.data.datasets[0].data = [option1, option2, option3];
    myChart.update();
  } catch (error) {
    console.error("Error:", error);
  }
});

// document.getElementById("sendData").addEventListener("click", async () => {
//   try {
//     const data = { animal1: "value", animal2: "value", animal3: "value" }; // Replace with the actual data you want to send
//     const response = await axios.post(
//       "http://192.168.37.119/update_labels",
//       data,
//       {
//         headers: {
//           "Content-Type": "application/json",
//         },
//       }
//     );
//     console.log("POST response:", response.data);
//   } catch (error) {
//     console.error("Error:", error);
//   }
// });

// document.getElementById("sendData").addEventListener("click", async () => {
//   const esp32_ip = "http://192.168.37.119/update_labels";
//   const data = {
//     animal1: "K",
//     animal2: "N",
//     animal3: "E",
//   };
//   console.log("Sending POST request to:", esp32_ip);
//   console.log("Data:", data);
//   fetch(esp32_ip, {
//     method: "POST",
//     headers: {
//       "Content-Type": "application/json",
//     },
//     body: JSON.stringify(data),
//   })
//     .then((response) => {
//       console.log("Received response:", response);
//       if (response.ok) {
//         return response.text();
//       } else {
//         throw new Error(
//           `Failed to update labels. HTTP Status Code: ${response.status}`
//         );
//       }
//     })
//     .then((responseText) => {
//       console.log("Labels updated successfully!");
//       console.log("Response:", responseText);
//     })
//     .catch((error) => {
//       console.error("An error occurred:", error);
//     });

//   try {
//     const data = { animal1: "value", animal2: "value", animal3: "value" }; // Replace with the actual data you want to send
//     const response = await fetch("http://192.168.37.119/update_labels", {
//       method: "POST",
//       headers: {
//         "Content-Type": "application/json",
//       },
//       body: JSON.stringify(data),
//     });
//     if (!response.ok) throw new Error("Network response was not ok");
//     const text = await response.text();
//     console.log("POST response:", text);
//   } catch (error) {
//     console.error("Error:", error);
//   }
// });
console.log(buttonArray);
