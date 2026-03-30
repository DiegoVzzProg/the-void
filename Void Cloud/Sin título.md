import Logo from '@/public/logo.svg'

const Navbar = () => {
  return (
    <nav>
      {/* TypeScript reconocerá className, width, height, etc. */}
      <Logo 
        className="h-8 w-8 text-blue-600" 
        title="Logo de la empresa" 
      />
    </nav>
  )
}