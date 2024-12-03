# Importa la biblioteca Dash y componentes adicionales para la creación de aplicaciones web interactivas.
import dash
from dash import html, dcc, Input, Output, State
# Importa Plotly para la creación de gráficos interactivos.
import plotly.graph_objs as go
# Importa la biblioteca json para la manipulación de datos en formato JSON.
import json
# Importa funciones del archivo mqtt_worker para la comunicación MQTT.
from mqtt_worker import client, send_json_to_topic

# Crea una instancia de la aplicación Dash.
app = dash.Dash(__name__)

# Define estilos CSS en un diccionario de Python para usarlos en la aplicación.
styles = {
    'device_container': {
        'border': '3px solid #999',
        'margin': '50px auto',
        'padding': '50px',
        'border-radius': '15px',
        'width': '80%',
        'display': 'flex',
        'flex-wrap': 'wrap',
        'justify-content': 'center',
        'align-items': 'center',
        'background-color': '#222',
        'color': '#fff'
    },
    'graph_container': {
        'width': '40%',
        'display': 'inline-block',
        'border': '2px solid #555',
        'border-radius': '10px',
        'padding': '5px',
        'box-shadow': '0 2px 6px rgba(0,0,0,0.3)',
        'margin': '20px',
        'background-color': '#333'
    },
    'control_panel': {
        'width': '10%',
        'background-color': '#555',
        'padding': '20px',
        'color': '#fff',
        'display': 'flex',
        'flex-direction': 'column',
        'align-items': 'center',
        'margin-left': '10px'
    }
}

# Define el layout de la aplicación utilizando componentes HTML y Dash.
app.layout = html.Div([
    html.H1("Dashboard Dinámico de Dispositivos", style={'color': '#111', 'text-align': 'center'}),
    dcc.Interval(id='interval-component', interval=1000, n_intervals=0),  # Timer para actualización periódica.
    html.Div([
        html.Div(id='graph-container', style=styles['device_container']),
        html.Div([
            html.H3("Control de Dispositivos", style={'textAlign': 'center', 'color': 'white'}),
            dcc.Input(id="input-1", type="text", placeholder="Ingrese el ID", style={'margin': '10px', 'width': '80%'}),
            dcc.Input(id="input-2", type="text", placeholder="Ingrese el valor PWM", style={'margin': '10px', 'width': '80%'}),
            html.Button("Enviar", id="send-button", n_clicks=0)
        ], style=styles['control_panel']),
        html.Div(id='dummy-div', style={'display': 'none'})  # Componente oculto para almacenar datos temporales.
    ], style={'display': 'flex', 'width': '100%'})
])

# Define una callback para manejar el evento de clic en el botón de enviar.
@app.callback(
    Output('dummy-div', 'children'),
    Input('send-button', 'n_clicks'),
    State('input-1', 'value'),
    State('input-2', 'value')
)
def print_input_values(n_clicks, input1, input2):
    if n_clicks > 0:  # Verifica si el botón fue presionado.
        send_json_to_topic(input1, input2)  # Envía datos a un dispositivo a través de MQTT.
        return ""  # Regresa un string vacío como un valor dummy.

def load_data():
    # Función para cargar datos desde un archivo JSON.
    with open('data.json', 'r') as file:
        data = json.load(file)
    return data

def generate_graphs(data):
    # Función para generar gráficos basados en los datos cargados.
    device_containers = []
    for device_id, details in data.items():
        graphs = []
        for prop in details["properties"]:
            if prop in details:
                x_values = list(range(len(details[prop])))
                y_values = details[prop]
                graph = dcc.Graph(
                    figure=go.Figure(
                        data=[go.Scatter(
                            x=x_values,
                            y=y_values,
                            mode='lines+markers',
                            name=device_id,
                            marker=dict(color='red')
                        )],
                        layout=go.Layout(
                            title=f"{prop.capitalize()} de {device_id}",
                            xaxis_title="Lectura No.",
                            yaxis_title=prop.capitalize(),
                            plot_bgcolor='#333',
                            paper_bgcolor='#333',
                            font=dict(color='#fff'),
                            height=300
                        )
                    ),
                    style=styles['graph_container']
                )
                graphs.append(graph)
        device_container = html.Div(graphs, style=styles['device_container'])
        device_containers.append(device_container)
    return device_containers

# Callback para actualizar gráficos de manera periódica.
@app.callback(
    Output('graph-container', 'children'),
    Input('interval-component', 'n_intervals')
)
def update_graphs(n):
    data = load_data()  # Carga los datos actualizados.
    return generate_graphs(data)  # Regenera los gráficos con los datos actualizados.

if __name__ == '__main__':
    app.run_server(debug=True, use_reloader=False)  # Ejecuta la aplicación.
