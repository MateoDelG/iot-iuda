import json  # Importa el módulo json para trabajar con archivos JSON.
import paho.mqtt.client as mqtt  # Importa el módulo paho.mqtt.client para trabajar con el protocolo MQTT.
import os  # Importa el módulo os, que proporciona funciones para interactuar con el sistema operativo.
import uuid
import webbrowser #abrir enlace desde consola



def update_json_file(data):
    filename = 'data.json'  # Define el nombre del archivo JSON donde se almacenarán los datos.
    # Comprueba si el archivo existe. Si existe, lo abre y carga los datos; si no, crea un diccionario vacío.
    if os.path.exists(filename):
        with open(filename, 'r') as file:
            existing_data = json.load(file)
    else:
        existing_data = {}

    device_id = data["id"]  # Extrae el ID del dispositivo de los datos recibidos.
    # Comprueba si el ID del dispositivo ya existe en los datos; si no, lo inicializa.
    if device_id not in existing_data:
        existing_data[device_id] = {
            "id": device_id,
            "properties": data["properties"]
        }
        # Inicializa listas vacías para cada propiedad del dispositivo.
        for prop in data["properties"]:
            existing_data[device_id][prop] = []

    # Agrega los valores a las listas de propiedades correspondientes.
    for prop, value in zip(data["properties"], data["values"]):
        if prop in existing_data[device_id]:
            existing_data[device_id][prop].append(value)
        else:
            existing_data[device_id][prop] = [value]

    # Guarda los datos modificados en el archivo JSON.
    with open(filename, 'w') as file:
        json.dump(existing_data, file, indent=4)

def delete_json_file(filename):
    # Comprueba si el archivo existe. Si existe, lo elimina; si no, muestra un mensaje.
    if os.path.exists(filename):
        os.remove(filename)
        print(f"Archivo {filename} borrado.")
    else:
        print(f"No se encontró el archivo {filename} para borrar.")

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conectado exitosamente.")
        client.subscribe("ae3/topic")
    else:
        print(f"Fallo al conectar. Código de retorno: {rc}")

def on_message(client, userdata, msg):
    # Función que se llama cuando se recibe un mensaje en el tópico suscrito.
    try:
        data = json.loads(msg.payload.decode())  # Intenta decodificar el mensaje JSON.
        # Verifica si los datos son válidos antes de actualizar el archivo JSON.
        print(data, flush=True)
        if "id" in data and "properties" in data and "values" in data and len(data["properties"]) == len(data["values"]):
            update_json_file(data)
    except json.JSONDecodeError:
        print("Error decoding JSON", flush=True)  # Maneja errores de decodificación JSON.
    except Exception as e:
        print(f"An error occurred: {e}", flush=True)  # Maneja cualquier otro tipo de error.

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print(f"Desconectado inesperadamente. Código de retorno: {rc}")

#Limpia la "base de datos" data.json
delete_json_file("data.json")



# Inicializa el cliente MQTT y configura las funciones de conexión y mensaje.
try:
    # Intenta crear un cliente MQTT especificando la versión del API.
    unique_client_id = f"Actividad_3_{uuid.uuid4().hex[:8]}"
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, unique_client_id)

except AttributeError:
    # Si ocurre un error porque la versión del API no existe, se crea un cliente sin especificar la versión.
    client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect


print(">>>>>>>>>>>>>>>Actividad: Uso de API<<<<<<<<<<<<<<<")
print("\n\n")
print("[!] Datos del broker (para el ESP32):\n URL: broker.mqtt.cool\n Puerto: 1883")
# print(f"Haz clic en el siguiente enlace: {url}")
print("[!] URL del broker para enviar datos de prueba: https://testclient-cloud.mqtt.cool/")
webbrowser.open("https://testclient-cloud.mqtt.cool/")  # Abre el enlace en el navegador predeterminado
print("[!]Topic al cual debes publicar: ae3/topic")
print("\n")
print('[!] Utiliza la siguiente estructura JSON:\n{\n"id": "carnet_o_cedula",\n"properties": ["nombre_variable1", "nombre_variable2", ...],\n"values": [valor_variable1, valor_variable2, ...]\n}')
print('[!] Por ejemplo:\n{\n"id": "12345678",\n"properties": ["temperatura", "humedad"],\n"values": [23, 65]\n}')
print("\n")


# Conecta al cliente con el servidor MQTT y comienza el bucle para procesar mensajes de manera indefinida.
client.connect("broker.mqtt.cool", 1883, keepalive=300)
try:
    client.loop_forever()
except Exception as e:
    print(f"Error en el bucle principal: {e}")


"""
Utiliza la siguiente estructura JSON:
{"id": "carnet_o_cedula",
"properties": ["nombre_variable1", "nombre_variable2", ...],
"values": [valor_variable1, valor_variable2, ...]}

Por ejemplo:
{"id": "12345678",
"properties": ["temperatura", "humedad"],
"values": [23, 65]}

Topic al cual te debes suscribir: ae3/topic

Datos del broker:
    URL: broker.mqtt.cool
    Puerto: 1883
"""