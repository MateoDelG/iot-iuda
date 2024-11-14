import json  # Importa el módulo json para trabajar con archivos JSON.
import paho.mqtt.client as mqtt  # Importa el módulo paho.mqtt.client para trabajar con el protocolo MQTT.
import os  # Importa el módulo os, que proporciona funciones para interactuar con el sistema operativo.

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
    # Función que se llama cuando el cliente se conecta al servidor MQTT.
    print("Connected with result code " + str(rc))
    client.subscribe("ae3/topic")  # El cliente se suscribe a un tópico específico.

def on_message(client, userdata, msg):
    # Función que se llama cuando se recibe un mensaje en el tópico suscrito.
    try:
        data = json.loads(msg.payload.decode())  # Intenta decodificar el mensaje JSON.
        # Verifica si los datos son válidos antes de actualizar el archivo JSON.
        print(data)
        if "id" in data and "properties" in data and "values" in data and len(data["properties"]) == len(data["values"]):
            update_json_file(data)
    except json.JSONDecodeError:
        print("Error decoding JSON")  # Maneja errores de decodificación JSON.
    except Exception as e:
        print(f"An error occurred: {e}")  # Maneja cualquier otro tipo de error.

#Limpia la "base de datos" data.json
delete_json_file("data.json")



# Inicializa el cliente MQTT y configura las funciones de conexión y mensaje.
try:
    # Intenta crear un cliente MQTT especificando la versión del API.
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, "Actividad_3")
except AttributeError:
    # Si ocurre un error porque la versión del API no existe, se crea un cliente sin especificar la versión.
    client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

# Conecta al cliente con el servidor MQTT y comienza el bucle para procesar mensajes de manera indefinida.
client.connect("broker.mqtt.cool", 1883, 60)
client.loop_forever()


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