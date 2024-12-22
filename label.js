let data = new URLSearchParams({
  animal1: barData[0],
  animal2: barData[1],
  animal3: barData[2],
});

console.log("Sending POST request to:", "http://192.168.37.119/update_labels");
console.log("Data:", data.toString());

fetch("http://192.168.37.119/update_labels", {
  method: "POST",
  headers: {
    "Content-Type": "application/x-www-form-urlencoded",
  },
  body: data.toString(),
})
  .then((response) => {
    console.log("Received response:", response);
    if (response.ok) {
      return response.text();
    } else {
      return response.text().then((text) => {
        throw new Error(
          `Failed to update labels. HTTP Status Code: ${response.status}. Response: ${text}`
        );
      });
    }
  })
  .then((responseText) => {
    console.log("Labels updated successfully!");
    console.log("Response:", responseText);
  })
  .catch((error) => {
    console.error("An error occurred:", error);
  });
