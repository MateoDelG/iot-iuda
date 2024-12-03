# Importa las bibliotecas necesarias para el manejo de MQTT, JSON y operaciones del sistema operativo.
import paho.mqtt.client as mqtt
import json
import os

# Ejemplo de estructura para el envío de datos
# {"id": "12345678",
# "properties": ["temperatura", "humedad"],
# "values": [23, 65]}

# Estructura para la recepción de datos
# {"id": "el_mismo_id_de_envio", "pwm": "valor_de_0_a_100"}
# Ejemplo de estructura para la recepción de datos
# {"id": "12345678", "pwm": "444"}

# Configura una variable global para el cliente MQTT.
client = None

def update_json_file(data):
    # Define el nombre del archivo JSON donde se almacenarán los datos.
    filename = 'data.json'
    
    # Comprueba si el archivo existe. Si existe, lo abre y carga los datos; si no, crea un diccionario vacío.
    if os.path.exists(filename):
        with open(filename, 'r') as file:
            existing_data = json.load(file)
    else:
        existing_data = {}

    # Extrae el ID del dispositivo de los datos recibidos y lo utiliza para actualizar el archivo.
    device_id = data["id"]
    
    # Comprueba si el ID del dispositivo ya está registrado, y si no, inicializa su estructura de datos.
    if device_id not in existing_data:
        existing_data[device_id] = {
            "id": device_id,
            "properties": data["properties"]
        }
        
        # Inicializa listas vacías para cada propiedad del dispositivo para almacenar valores futuros.
        for prop in data["properties"]:
            existing_data[device_id][prop] = []

    # Agrega los nuevos valores a las listas de propiedades del dispositivo.
    for prop, value in zip(data["properties"], data["values"]):
        existing_data[device_id][prop].append(value)

    # Escribe los datos actualizados de vuelta al archivo JSON, formateándolo para mejorar la legibilidad.
    with open(filename, 'w') as file:
        json.dump(existing_data, file, indent=4)

def delete_json_file(filename):
    # Comprueba si el archivo especificado existe y lo elimina si es así; de lo contrario, muestra un mensaje.
    if os.path.exists(filename):
        os.remove(filename)
        print(f"Archivo {filename} borrado.")
    else:
        print(f"No se encontró el archivo {filename} para borrar.")

def on_connect(client, userdata, flags, rc):
    # Se llama a esta función cuando el cliente MQTT se conecta al broker.
    print("Connected with result code " + str(rc))
    client.subscribe("ae4/topic")  # Se suscribe a un tópico MQTT para recibir mensajes.

def on_message(client, userdata, msg):
    # Esta función maneja los mensajes recibidos en el tópico suscrito.
    try:
        # Intenta decodificar el mensaje JSON recibido.
        data = json.loads(msg.payload.decode())
        print("Data arrived: ", data)
        # Actualiza el archivo JSON si los datos son válidos.
        if "id" in data and "properties" in data and "values" in data and len(data["properties"]) == len(data["values"]):
            update_json_file(data)
    except json.JSONDecodeError:
        print("Error decoding JSON")  # Maneja errores de decodificación JSON.
    except Exception as e:
        print(f"An error occurred: {e}")  # Maneja otros errores.

def setup_mqtt_client():
    global client
    try:
        # Crea un cliente MQTT utilizando una versión específica de la API.
        client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, "Actividad_4")
    except AttributeError:
        # Crea un cliente MQTT estándar si la versión especificada no está disponible.
        client = mqtt.Client()

    client.on_connect = on_connect
    client.on_message = on_message
    # https://testclient-cloud.mqtt.cool
    client.connect("broker.mqtt.cool", 1883, 60)  # Conecta al broker MQTT.
    client.loop_start()  # Inicia un bucle en el fondo para manejar las conexiones de red.

def send_json_to_topic(uuid, pwm):
    # Prepara los datos en formato JSON para enviarlos a un tópico MQTT.
    topic = "ae4/pwm"
    data_to_send = {"id": uuid, "pwm": pwm}
    json_data = json.dumps(data_to_send)
    try:
        # Publica los datos en el tópico MQTT especificado.
        client.publish(topic, json_data)
        print("Data sent to:", topic, json_data)
    except:
        print("Error sending data")

# Inicializa el cliente MQTT y lo configura para la comunicación.
setup_mqtt_client()
