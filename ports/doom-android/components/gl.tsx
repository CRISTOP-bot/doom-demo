import { requireNativeComponent, ViewStyle } from 'react-native';

interface DoomGLViewProps {
  style?: ViewStyle;
  surfaceWidth?: number;
  surfaceHeight?: number;
}

// El string debe coincidir EXACTO con DoomViewManager.NAME ("DoomGLView").
// Si no coincide, falla en tiempo de ejecucion con un error de
// "requireNativeComponent: DoomGLView was not found" — no en build time.
const DoomGLView = requireNativeComponent<DoomGLViewProps>('DoomGLView');

export default DoomGLView;