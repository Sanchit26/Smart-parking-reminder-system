from flask import Flask, request, jsonify
import easyocr
import requests
import os

app = Flask(_name_)

reader = easyocr.Reader(['en'])

# License plate to phone number mapping
license_to_phone = {
    "AB": "+919100770398",
    "XY": "+916303447534"
}

# Meta WhatsApp Cloud API credentials
ACCESS_TOKEN = "YOUR_META_ACCESS_TOKEN"
PHONE_NUMBER_ID = "YOUR_PHONE_NUMBER_ID"

def send_whatsapp(phone, message):
    url = f"https://graph.facebook.com/v19.0/{PHONE_NUMBER_ID}/messages"
    headers = {
        "Authorization": f"Bearer {ACCESS_TOKEN}",
        "Content-Type": "application/json"
    }
    payload = {
        "messaging_product": "whatsapp",
        "to": phone,
        "type": "text",
        "text": {
            "body": message
        }
    }
    response = requests.post(url, headers=headers, json=payload)
    print("WhatsApp API response:", response.status_code, response.text)
    return response.status_code == 200

@app.route('/', methods=['POST'])
def receive_image():
    try:
        image_data = request.data
        if not image_data:
            return jsonify({"error": "No image received"}), 400

        # Save image temporarily
        with open("temp.jpg", "wb") as f:
            f.write(image_data)

        # OCR to read plate
        results = reader.readtext("temp.jpg", detail=0)
        print("OCR Results:", results)

        plate = None
        for text in results:
            clean = text.replace(" ", "").upper()
            if len(clean) >= 2:
                plate = clean
                break

        if not plate:
            return jsonify({"error": "License plate not found"}), 404

        phone = license_to_phone.get(plate)
        if not phone:
            return jsonify({"error": f"No phone mapped for plate: {plate}"}), 404

        message = f"Hello, your vehicle with plate {plate} is parked. Reminder."

        print(f"Sending WhatsApp to {phone} - {message}")
        success = send_whatsapp(phone, message)

        if success:
            return jsonify({"plate": plate, "phone": phone, "status": "WhatsApp sent"}), 200
        else:
            return jsonify({"error": "Failed to send WhatsApp"}), 500

    except Exception as e:
        print("Error:", e)
        return jsonify({"error": str(e)}), 500

    finally:
        if os.path.exists("temp.jpg"):
            os.remove("temp.jpg")

if _name_ == '_main_':
    print("🚀 Server running — Meta WhatsApp API mode.")
    app.run(host='0.0.0.0', port=5000)