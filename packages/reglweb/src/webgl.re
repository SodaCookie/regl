/*
 * vim: set ft=rust:
 * vim: set ft=reason:
 */
module Document = {
  type element;
  type window;
  let window: window = [%bs.raw "window"];
  /* external setGlDebug : window => GlT.context => unit = "debugContext" [@@bs.set]; */
  external getElementById : string => element = "document.getElementById" [@@bs.val];
  external getContext : element => string => 'context = "getContext" [@@bs.send];
  external getWidth : element => int = "width" [@@bs.get];
  external getHeight : element => int = "height" [@@bs.get];
  external requestAnimationFrame : (unit => unit) => unit = "window.requestAnimationFrame" [@@bs.val];
  external now : unit => float = "Date.now" [@@bs.val];
  external addEventListener : 'window => string => ('eventT => unit) => unit = "addEventListener" [@@bs.send];
};

external getButton : 'eventT => int = "button" [@@bs.get];

external getClientX : 'eventT => int = "clientX" [@@bs.get];

external getClientY : 'eventT => int = "clientY" [@@bs.get];

external getWidth : 'canvas => int = "width" [@@bs.get];

external getHeight : 'canvas => int = "height" [@@bs.get];

external setWidth : 'canvas => int => unit = "width" [@@bs.set];

external setHeight : 'canvas => int => unit = "height" [@@bs.set];

external createCanvas : string => 'canvas = "document.createElement" [@@bs.val];

let createCanvas () => createCanvas "canvas";

external addToBody : 'canvas => unit = "document.body.appendChild" [@@bs.val];

external getContext : 'canvas => string => 'context = "getContext" [@@bs.send];

module Gl: Reglinterface.Gl.t = {
  let target = "web";
  type contextT;
  module type WindowT = {
    type t;
    let getWidth: t => int;
    let getHeight: t => int;
    let init: argv::array string => t;
    let setWindowSize: window::t => width::int => height::int => unit;
    let initDisplayMode: window::t => double_buffer::bool => unit => unit;
    let getContext: t => contextT;
  };
  module Window = {
    type t;
    let getWidth = getWidth;
    let getHeight = getHeight;
    let init argv::_ => {
      let canvas: t = createCanvas ();
      addToBody canvas;
      canvas
    };
    let setWindowSize window::(window: t) ::width ::height => {
      setWidth window width;
      setHeight window height
    };
    let initDisplayMode ::window double_buffer::_ () => ();
    let getContext (window: t) :contextT => getContext window "webgl";
  };
  module type EventsT = {
    type buttonStateT =
      | LEFT_BUTTON
      | MIDDLE_BUTTON
      | RIGHT_BUTTON;
    type stateT =
      | DOWN
      | UP;
  };
  module Events = {
    type buttonStateT =
      | LEFT_BUTTON
      | MIDDLE_BUTTON
      | RIGHT_BUTTON;
    type stateT =
      | DOWN
      | UP;
  };
  type mouseButtonEventT =
    button::Events.buttonStateT => state::Events.stateT => x::int => y::int => unit;

  /** See Gl.re for explanation. **/
  let render
      window::(window: Window.t)
      mouseDown::(mouseDown: option mouseButtonEventT)=?
      mouseUp::(mouseUp: option mouseButtonEventT)=?
      mouseMove::(mouseMove: option (x::int => y::int => unit))=?
      windowResize::(windowResize: option (unit => unit))=?
      displayFunc::(displayFunc: float => unit)
      () => {
    switch mouseDown {
    | None => ()
    | Some cb =>
      Document.addEventListener
        window
        "mousedown"
        (
          fun e => {
            let button =
              switch (getButton e) {
              | 0 => Events.LEFT_BUTTON
              | 1 => Events.MIDDLE_BUTTON
              | 2 => Events.RIGHT_BUTTON
              | _ => assert false
              };
            let state = Events.DOWN;
            let x = getClientX e;
            let y = getClientY e;
            cb ::button ::state ::x ::y
          }
        )
    };
    switch mouseUp {
    | None => ()
    | Some cb =>
      Document.addEventListener
        window
        "mouseup"
        (
          fun e => {
            let button =
              switch (getButton e) {
              | 0 => Events.LEFT_BUTTON
              | 1 => Events.MIDDLE_BUTTON
              | 2 => Events.RIGHT_BUTTON
              | _ => assert false
              };
            let state = Events.UP;
            let x = getClientX e;
            let y = getClientY e;
            cb ::button ::state ::x ::y
          }
        )
    };
    switch mouseMove {
    | None => ()
    | Some cb =>
      Document.addEventListener
        window
        "mousemove"
        (
          fun e => {
            let x = getClientX e;
            let y = getClientY e;
            cb ::x ::y
          }
        )
    };
    let rec tick prev () => {
      let now = Document.now ();
      displayFunc (now -. prev);
      Document.requestAnimationFrame (tick now)
    };
    Document.requestAnimationFrame (tick (Document.now ()))
  };
  type programT;
  type shaderT;
  external clearColor : context::contextT => r::float => g::float => b::float => a::float => unit = "clearColor" [@@bs.send];
  external createProgram : context::contextT => programT = "createProgram" [@@bs.send];
  external createShader : context::contextT => shaderType::int => shaderT = "createShader" [@@bs.send];
  external _shaderSource : context::contextT => shader::shaderT => source::string => unit = "shaderSource" [@@bs.send];
  let shaderSource ::context ::shader ::source =>
    _shaderSource
      ::context ::shader source::("#version 100 \n precision highp float; \n" ^ source);
  external compileShader : context::contextT => shader::shaderT => unit = "compileShader" [@@bs.send];
  external attachShader : context::contextT => program::programT => shader::shaderT => unit = "attachShader" [@@bs.send];
  external deleteShader : context::contextT => shader::shaderT => unit = "deleteShader" [@@bs.send];
  external linkProgram : context::contextT => program::programT => unit = "linkProgram" [@@bs.send];
  external useProgram : context::contextT => program::programT => unit = "useProgram" [@@bs.send];
  type bufferT;
  type attributeT;
  type uniformT;
  external createBuffer : context::contextT => bufferT = "createBuffer" [@@bs.send];
  external bindBuffer : context::contextT => target::int => buffer::bufferT => unit = "bindBuffer" [@@bs.send];
  type textureT;
  external createTexture : context::contextT => textureT = "createTexture" [@@bs.send];
  external activeTexture : context::contextT => target::int => unit = "activeTexture" [@@bs.send];
  external bindTexture : context::contextT => target::int => texture::textureT => unit = "bindTexture" [@@bs.send];
  external texParameteri : context::contextT => target::int => pname::int => param::int => unit = "texParameteri" [@@bs.send];
  type rawTextureDataT = array int;
  external makeUint8Array : array int => rawTextureDataT = "Uint8Array" [@@bs.new];
  let toTextureData data => makeUint8Array data;
  external enable : context::contextT => int => unit = "enable" [@@bs.send];
  external disable : context::contextT => int => unit = "disable" [@@bs.send];
  external blendFunc : context::contextT => int => int => unit = "blendFunc" [@@bs.send];
  type imageT;
  external getImageWidth : imageT => int = "width" [@@bs.get];
  external getImageHeight : imageT => int = "height" [@@bs.get];
  type loadOptionT =
    | LoadAuto
    | LoadL
    | LoadLA
    | LoadRGB
    | LoadRGBA;

  /** makeImage, setSrc and addEventListener are three helpers for loadImage. */
  external makeImage : unit => imageT = "Image" [@@bs.new];
  /* TODO: when the type of bs.set is `imageT => unit` you get something like

      node_modules/reglweb/src/webgl.re:
        Bsppx.Location.Error(_)
        File "node_modules/reglweb/src/webgl.re", line 1:
        Error: Error while running external preprocessor
        Command line: bsppx.exe '/var/folders/g_/v45pqsrn65xbszd33yvd8lj40000gn/T/camlppx129341' '/var/folders/g_/v45pqsrn65xbszd33yvd8lj40000gn/T/camlppx438846'

        This is due to the fact that bsppx expects 2 args, the "this" and the new value to set.

        We should open an issue in Buckelscript.
     */
  external setSrc : imageT => string => unit = "src" [@@bs.set];
  external addEventListener : imageT => string => (unit => unit) => unit = "addEventListener" [@@bs.send];

  /** TODO: We don't care about forcing load option for web images (we do allow it for native as SOIL supports
      it). We should probably not do this... */
  let loadImage ::filename ::loadOption=? ::callback () =>
    switch loadOption {
    | _ =>
      let image = makeImage ();
      setSrc image filename;
      addEventListener image "load" (fun () => callback (Some image))
    };
  external _texImage2DWithImage : context::contextT =>
                                  target::int =>
                                  level::int =>
                                  internalFormat::int =>
                                  format::int =>
                                  type_::int =>
                                  image::imageT =>
                                  unit = "texImage2D" [@@bs.send];
  let texImage2DWithImage ::context ::target ::level ::image =>
    _texImage2DWithImage
      context
      target
      level
      Reglinterface.Constants.rgba
      Reglinterface.Constants.rgba
      Reglinterface.Constants.unsigned_byte
      image;
  external _texImage2D : context::contextT =>
                         target::int =>
                         level::int =>
                         internalFormat::int =>
                         width::int =>
                         height::int =>
                         border::int =>
                         format::int =>
                         type_::int =>
                         data::rawTextureDataT =>
                         unit = "texImage2D" [@@bs.send];
  let texImage2D
      ::context
      ::target
      ::level
      ::internalFormat
      ::width
      ::height
      ::format
      ::type_
      ::data =>
    _texImage2D context target level internalFormat width height 0 format type_ data;
  external generateMipmap : context::contextT => target::int => unit = "generateMipmap" [@@bs.send];

  /** Those types are what allows to come close to some form of ad-hoc polymorphism
   *  See the Bucklescript manual:
   *  https://bloomberg.github.io/bucklescript/Manual.html#_phantom_arguments_and_ad_hoc_polyrmophism
   */
  type float32Array = array float;
  type uint16Array = array int;
  type dataKind =
    | Float32 float32Array
    | UInt16 uint16Array;

  /** Those externals are used for bufferData to instantiate what gl.bufferData actually expects, because JS
   *  doesn't differentiate between float and int but the GL backend needs to know the types precisely.
   **/
  external createFloat32Array : array float => 'float32Array = "Float32Array" [@@bs.new];
  external createUint16Array : array int => 'uint16Array = "Uint16Array" [@@bs.new];
  external _bufferData : context::contextT => target::int => data::array 'a => usage::int => unit = "bufferData" [@@bs.send];
  let bufferData ::context ::target ::data ::usage =>
    switch data {
    | Float32 x => _bufferData ::context ::target data::(createFloat32Array x) ::usage
    | UInt16 x => _bufferData ::context ::target data::(createUint16Array x) ::usage
    };
  external viewport : context::contextT => x::int => y::int => width::int => height::int => unit = "viewport" [@@bs.send];
  external clear : context::contextT => mask::int => unit = "clear" [@@bs.send];
  external getUniformLocation : context::contextT => program::programT => name::string => uniformT = "getUniformLocation" [@@bs.send];
  external getAttribLocation : context::contextT => program::programT => name::string => attributeT = "getAttribLocation" [@@bs.send];
  external enableVertexAttribArray : context::contextT => attribute::attributeT => unit = "enableVertexAttribArray" [@@bs.send];
  external _vertexAttribPointer : context::contextT =>
                                  attribute::attributeT =>
                                  size::int =>
                                  type_::int =>
                                  normalize::Js.boolean =>
                                  stride::int =>
                                  offset::int =>
                                  unit = "vertexAttribPointer" [@@bs.send];
  let vertexAttribPointer ::context ::attribute ::size ::type_ ::normalize ::stride ::offset => {
    let normalize = if normalize {Js.true_} else {Js.false_};
    _vertexAttribPointer ::context ::attribute ::size ::type_ ::normalize ::stride ::offset
  };
  module type Mat4T = {
    type t;
    let to_array: t => array float;
    let create: unit => t;
    let identity: out::t => unit;
    let translate: out::t => matrix::t => vec::array float => unit;
    let scale: out::t => matrix::t => vec::array float => unit;
    let rotate: out::t => matrix::t => rad::float => vec::array float => unit;
    let ortho:
      out::t =>
      left::float =>
      right::float =>
      bottom::float =>
      top::float =>
      near::float =>
      far::float =>
      unit;
  };
  module Mat4: Mat4T = {
    type t = array float;
    let to_array a => a;
    external create : unit => t = "mat4.create" [@@bs.val];
    external identity : out::t => unit = "mat4.identity" [@@bs.val];
    external translate : out::t => matrix::t => vec::array float => unit = "mat4.translate" [@@bs.val];
    external scale : out::t => matrix::t => vec::array float => unit = "mat4.scale" [@@bs.val];
    external rotate : out::t => matrix::t => rad::float => vec::array float => unit = "mat4.rotate" [@@bs.val];
    external ortho : out::t =>
                     left::float =>
                     right::float =>
                     bottom::float =>
                     top::float =>
                     near::float =>
                     far::float =>
                     unit = "mat4.ortho" [@@bs.val];
  };
  external uniform1i : context::contextT => location::uniformT => int => unit = "uniform1i" [@@bs.send];
  external uniform1f : context::contextT => location::uniformT => float => unit = "uniform1f" [@@bs.send];
  external _uniformMatrix4fv : context::contextT =>
                               location::uniformT =>
                               transpose::Js.boolean =>
                               value::Mat4.t =>
                               unit = "uniformMatrix4fv" [@@bs.send];
  let uniformMatrix4fv ::context ::location ::value =>
    _uniformMatrix4fv ::context ::location transpose::Js.false_ ::value;
  /* Can return other value types as well, see https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Types */
  type shaderParamsInternalT 'a =
    | Shader_delete_status_internal :shaderParamsInternalT bool
    | Compile_status_internal :shaderParamsInternalT bool
    | Shader_type_internal :shaderParamsInternalT int;
  type programParamsInternalT 'a =
    | Program_delete_status_internal :programParamsInternalT bool
    | Link_status_internal :programParamsInternalT bool
    | Validate_status_internal :programParamsInternalT bool;
  /* | Attached_shaders_internal :programParamsInternalT int
     | Active_attributes_internal :programParamsInternalT int
     | Active_uniforms_internal :programParamsInternalT int */
  type shaderParamsT =
    | Shader_delete_status
    | Compile_status
    | Shader_type;
  type programParamsT =
    | Program_delete_status
    | Link_status
    | Validate_status;
  /* | Attached_shaders
     | Active_attributes
     | Active_uniforms */
  external deleteStatus : context::contextT => int = "DELETE_STATUS" [@@bs.get];
  external compileStatus : context::contextT => int = "COMPILE_STATUS" [@@bs.get];
  external linkStatus : context::contextT => int = "LINK_STATUS" [@@bs.get];
  external validateStatus : context::contextT => int = "VALIDATE_STATUS" [@@bs.get];
  external shaderType : context::contextT => int = "SHADER_TYPE" [@@bs.get];
  external _getProgramParameter : context::contextT =>
                                  program::programT =>
                                  paramName::int =>
                                  (programParamsInternalT 'a) [@bs.ignore] =>
                                  'a = "getProgramParameter" [@@bs.send];
  let getProgramParameter ::context ::program ::paramName =>
    switch paramName {
    | Program_delete_status =>
      if (
        _getProgramParameter
          ::context ::program paramName::(deleteStatus ::context) Program_delete_status_internal
      ) {
        1
      } else {
        0
      }
    | Link_status =>
      if (
        _getProgramParameter
          ::context ::program paramName::(linkStatus ::context) Link_status_internal
      ) {
        1
      } else {
        0
      }
    | Validate_status =>
      if (
        _getProgramParameter
          ::context ::program paramName::(validateStatus ::context) Validate_status_internal
      ) {
        1
      } else {
        0
      }
    };
  external _getShaderParameter : context::contextT =>
                                 shader::shaderT =>
                                 paramName::int =>
                                 (shaderParamsInternalT 'a) [@bs.ignore] =>
                                 'a = "getShaderParameter" [@@bs.send];
  let getShaderParameter ::context ::shader ::paramName =>
    switch paramName {
    | Shader_delete_status =>
      if (
        _getShaderParameter
          ::context ::shader paramName::(deleteStatus ::context) Shader_delete_status_internal
      ) {
        1
      } else {
        0
      }
    | Compile_status =>
      if (
        _getShaderParameter
          ::context ::shader paramName::(compileStatus ::context) Compile_status_internal
      ) {
        1
      } else {
        0
      }
    | Shader_type =>
      _getShaderParameter ::context ::shader paramName::(shaderType ::context) Shader_type_internal
    };
  external getShaderInfoLog : context::contextT => shader::shaderT => string = "getShaderInfoLog" [@@bs.send];
  external getProgramInfoLog : context::contextT => program::programT => string = "getProgramInfoLog" [@@bs.send];
  external getShaderSource : context::contextT => shader::shaderT => string = "getShaderSource" [@@bs.send];
  external drawArrays : context::contextT => mode::int => first::int => count::int => unit = "drawArrays" [@@bs.send];
  external drawElements : context::contextT =>
                          mode::int =>
                          count::int =>
                          type_::int =>
                          offset::int =>
                          unit = "drawElements" [@@bs.send];
};
