import dash  # Importa la biblioteca Dash para la creación de aplicaciones web.
from dash import html, dcc, Input, Output  # Importa componentes específicos de Dash para HTML y interactividad.
import plotly.graph_objs as go  # Importa objetos gráficos de Plotly para la visualización de datos.
import json  # Importa la biblioteca json para cargar y manipular archivos JSON.
import webbrowser  # Importa el módulo para abrir el navegador automáticamente.

app = dash.Dash(__name__)  # Crea una nueva aplicación Dash.

# Define estilos CSS en forma de diccionario de Python para usar en la aplicación.
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
    }
}

# Configura el layout principal de la aplicación usando componentes HTML y Dash.
app.layout = html.Div([
    html.H1("Dashboard Dinámico de Dispositivos", style={'color': '#111', 'text-align': 'center'}),
    dcc.Interval(id='interval-component', interval=1000, n_intervals=0),  # Intervalo para actualizar los datos.
    html.Div(id='graph-container', style={'background-color': '#444'})  # Contenedor para los gráficos.
])

def load_data():
    # Carga datos desde un archivo JSON y los retorna como un diccionario de Python.
    with open('data.json', 'r') as file:
        data = json.load(file)
    return data

def generate_graphs(data):
    # Genera gráficos para cada dispositivo y propiedad usando los datos.
    device_containers = []
    for device_id, details in data.items():
        graphs = []
        for prop in details["properties"]:
            if prop in details:
                # Crea los valores X e Y para los gráficos.
                x_values = list(range(len(details[prop])))
                y_values = details[prop]
                # Configura el gráfico usando Plotly.
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

# Define un callback para actualizar los gráficos cada vez que el intervalo se dispara.
@app.callback(
    Output('graph-container', 'children'),
    Input('interval-component', 'n_intervals')
)
def update_graphs(n):
    # Carga los datos y genera gráficos con los datos actualizados.
    data = load_data()
    return generate_graphs(data)


run_one_time = False
def open_dashboard():
    global run_one_time 
    # Define la dirección del servidor
    host = "127.0.0.1"  # Dirección local
    port = 8050         # Puerto del servidor
    dashboard_url = f"http://{host}:{port}/"  # URL completa del dashboard

    # Imprime la URL en consola
    print("Actividad: Uso de API-> Dashboard Dinámico de Dispositivos")
    print(f"[!] El dashboard está disponible en: {dashboard_url}")
    # Abre el navegador automáticamente
    if(not run_one_time):
        webbrowser.open(dashboard_url)
        run_one_time = True
        
if __name__ == '__main__':
    # Define la dirección del servidor
    host = "127.0.0.1"  # Dirección local
    port = 8050         # Puerto del servidor
    # Ejecuta el servidor
    open_dashboard()
    app.run_server(debug=False, host=host, port=port)

