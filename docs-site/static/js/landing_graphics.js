document.addEventListener( 'DOMContentLoaded', () => {
    const canvas = document.querySelector( '.landing__graphic' )

    const compileShader = ( gl, sourceCode, shaderType ) => {
        const shader = gl.createShader( shaderType )
        gl.shaderSource( shader, sourceCode )
        gl.compileShader( shader )

        if ( !gl.getShaderParameter( shader, gl.COMPILE_STATUS ) ) {
            console.error( 
                'Shader compilation failed: ', 
                gl.getShaderInfoLog( shader ) )
            gl.deleteShader( shader )

            return null
        }

        return shader
    }

    const createProgram = ( gl, vertexShader, fragmentShader ) => {
        const program = gl.createProgram() 
        gl.attachShader( program, vertexShader )
        gl.attachShader( program, fragmentShader ) 
        gl.linkProgram( program)

        if ( !gl.getProgramParameter( program, gl.LINK_STATUS ) ) {
            console.error(
                'Program linking failed: ', 
                gl.getProgramInfoLog(program) )
            gl.deleteProgram(program)
            return null
        }

        return program
    }

    const main = () => {
        const gl = canvas.getContext( 'webgl', { alpha: true } )

        if ( !gl ) {
            console.error( 'WebGL not supported!' )
            return
        }

        const vertexShaderSource = `
            attribute vec2 position;
            varying vec2 v_tex_coords;
            void main() {
                v_tex_coords = position * 0.5 + 0.5;
                gl_Position = vec4(position, 0.0, 1.0);
            }`

        const fragmentShaderSource = `
            precision mediump float;
            varying vec2 v_tex_coords;
            uniform float time;
            uniform vec2 resolution;

            #define TAU 6.28318530718
            #define MAX_ITER 5

            void main() {
                vec2 uv = v_tex_coords;

                // Translate UV coordinates into screen space using resolution
                vec2 fragCoord = uv * resolution;
                
                float t = time * .5 + 24.0;

                vec2 p = mod(fragCoord.xy * TAU, TAU) - 250.0;
                vec2 i = vec2(p);
                float c = 1.0;
                float inten = .005;

                for (int n = 0; n < MAX_ITER; n++) {
                    float timeShift = t * (1.0 - (3.5 / float(n + 1)));
                    i = p + vec2(cos(timeShift - i.x) + sin(timeShift + i.y), 
                                sin(timeShift - i.y) + cos(timeShift + i.x));
                    c += 1.0 / length(vec2(p.x / (sin(i.x + timeShift) / inten), 
                                        p.y / (cos(i.y + timeShift) / inten)));
                }

                c /= float(MAX_ITER);
                c = 1.17 - pow(c, 1.4);

                vec3 color = vec3(pow(abs(c), 8.0));
                color = clamp(color + vec3(0.0, 0.0, 0.0), 0.0, 1.0);

                gl_FragColor = vec4(color, 1.0);
            }
            `

        const vertexShader = compileShader( 
            gl, vertexShaderSource, gl.VERTEX_SHADER )
        const fragmentShader = compileShader( 
            gl, fragmentShaderSource, gl.FRAGMENT_SHADER )

        const program = createProgram( gl, vertexShader, fragmentShader )

        const positionLocation = gl.getAttribLocation( program, 'position' )
        const timeLocation = gl.getUniformLocation( program, 'time' )
        const resolutionLocation = gl.getUniformLocation( program, 'resolution' )

        const positionBuffer = gl.createBuffer()
        gl.bindBuffer( gl.ARRAY_BUFFER, positionBuffer )
        const positions = new Float32Array([
            -1, -1,
            1, -1,
            -1,  1,
            1,  1,
        ])
        gl.bufferData( gl.ARRAY_BUFFER, positions, gl.STATIC_DRAW )

        gl.enableVertexAttribArray( positionLocation )
        gl.vertexAttribPointer( positionLocation, 2, gl.FLOAT, false, 0, 0 )

        const resizeCanvas = () => {
            canvas.width = window.innerWidth
            canvas.height = window.innerHeight
            gl.viewport( 0, 0, canvas.width, canvas.height )
        }

        window.addEventListener( 'resize', resizeCanvas )
        resizeCanvas()

        const render = ( time ) => {
            time *= 0.0005   // seconds

            gl.clearColor( 0.0, 0.0, 0.0, 0.0 ) 
            gl.clear( gl.COLOR_BUFFER_BIT )

            gl.useProgram( program )

            gl.uniform1f( timeLocation, time )
            gl.uniform2f( resolutionLocation, 1, 1 )

            gl.drawArrays( gl.TRIANGLE_STRIP, 0, 4)

            requestAnimationFrame( render )
        }

        requestAnimationFrame( render )
    }

    main()
})